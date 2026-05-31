// =============================================================================
// GamePage — Primary gameplay screen
// =============================================================================
// Constructed with a WorldMap reference that provides tile data, building
// interiors, and element interactions. Owns PlayerStats, GameTime, Inventory.
//
// Key interaction flow:
//   refreshTileContext → loads elements from map → refreshElementTable
//   user clicks action button → onElementAction → emits signal or applyGather
//
// Signals are routed through MainWindow which handles page switching
// (container page, map page, backpack page). This keeps GamePage decoupled
// from the other pages—it only needs to know what the player clicked.

#include "GamePage.h"
#include "WorldMap.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTimer>
#include <QVBoxLayout>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------
GamePage::GamePage(WorldMap *map, QWidget *parent)
    : QWidget(parent)
    , m_map(map)
{
    m_inventory = new Inventory(this);
    setupUI();
    setupTestItems();
}

// ---------------------------------------------------------------------------
// UI Setup
// ---------------------------------------------------------------------------
void GamePage::setupUI()
{
    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(12, 8, 12, 8);

    // ---- Top row: stats (left) + location info (right) ----
    auto *topRow = new QHBoxLayout();

    // -- Stats --
    auto *statsBox = new QVBoxLayout();
    statsBox->setSpacing(2);
    const QString statStyle = QStringLiteral(
        "QLabel{color:#e0e0e0;font-size:15px;font-family:monospace}");

    m_hpLabel    = new QLabel(this); m_hpLabel->setStyleSheet(statStyle);
    m_hungerLabel = new QLabel(this); m_hungerLabel->setStyleSheet(statStyle);
    m_thirstLabel = new QLabel(this); m_thirstLabel->setStyleSheet(statStyle);
    m_sanityLabel = new QLabel(this); m_sanityLabel->setStyleSheet(statStyle);
    m_restLabel  = new QLabel(this); m_restLabel->setStyleSheet(statStyle);

    statsBox->addWidget(m_hpLabel);
    statsBox->addWidget(m_hungerLabel);
    statsBox->addWidget(m_thirstLabel);
    statsBox->addWidget(m_sanityLabel);
    statsBox->addWidget(m_restLabel);
    statsBox->addStretch();
    topRow->addLayout(statsBox);

    topRow->addStretch();

    // -- Location info --
    auto *locBox = new QVBoxLayout();
    locBox->setSpacing(2);
    const QString locStyle = QStringLiteral(
        "QLabel{color:#c0c0d0;font-size:14px;font-family:monospace}");

    m_timeLabel = new QLabel(this); m_timeLabel->setStyleSheet(
        "QLabel{color:#e94560;font-size:16px;font-weight:bold;font-family:monospace}");
    m_tileLabel = new QLabel(this); m_tileLabel->setStyleSheet(locStyle);
    m_locLabel  = new QLabel(this); m_locLabel->setStyleSheet(locStyle);

    locBox->addWidget(m_timeLabel, 0, Qt::AlignRight);
    locBox->addWidget(m_tileLabel, 0, Qt::AlignRight);
    locBox->addWidget(m_locLabel, 0, Qt::AlignRight);
    locBox->addStretch();
    topRow->addLayout(locBox);

    outer->addLayout(topRow);

    // ---- Inventory full warning ----
    m_invFullLabel = new QLabel(QStringLiteral("背包已满，无法拾取更多物品！"), this);
    m_invFullLabel->setAlignment(Qt::AlignCenter);
    m_invFullLabel->setStyleSheet(QStringLiteral(
        "QLabel{color:#ff6b81;font-size:14px;padding:4px;"
        "background:#2a1a1a;border:1px solid #e94560;border-radius:4px}"));
    m_invFullLabel->setVisible(false);
    outer->addWidget(m_invFullLabel);

    // ---- Element table (center) ----
    m_elemTable = new QTableWidget(0, 2, this);
    m_elemTable->setHorizontalHeaderLabels(
        {QStringLiteral("名称"), QStringLiteral("功能")});
    m_elemTable->horizontalHeader()->setStretchLastSection(true);
    m_elemTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_elemTable->verticalHeader()->setVisible(false);
    m_elemTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_elemTable->setSelectionMode(QAbstractItemView::NoSelection);
    m_elemTable->setFocusPolicy(Qt::NoFocus);
    m_elemTable->setStyleSheet(QStringLiteral(
        "QTableWidget{background-color:#0f0f1a;color:#e0e0e0;"
        "gridline-color:#1a1a2e;border:1px solid #1a1a2e;font-size:14px}"
        "QHeaderView::section{background-color:#16213e;color:#c0c0d0;"
        "border:none;padding:6px;font-weight:bold}"));
    outer->addWidget(m_elemTable, 1);

    // ---- Bottom row: navigation buttons ----
    auto *bottomRow = new QHBoxLayout();
    const QString navBtnStyle = QStringLiteral(
        "QPushButton{background-color:#16213e;color:#c0c0d0;"
        "border:1px solid #0f3460;border-radius:6px;"
        "font-size:14px;padding:8px 16px;min-width:80px}"
        "QPushButton:hover{background-color:#0f3460;color:#ffffff;border-color:#e94560}");

    m_btnBigMap = new QPushButton(QStringLiteral("大地图"), this);
    m_btnBigMap->setStyleSheet(navBtnStyle);
    m_btnBigMap->setCursor(Qt::PointingHandCursor);

    m_btnSmallMap = new QPushButton(QStringLiteral("小地图"), this);
    m_btnSmallMap->setStyleSheet(navBtnStyle);
    m_btnSmallMap->setCursor(Qt::PointingHandCursor);

    m_btnBackpack = new QPushButton(QStringLiteral("背包"), this);
    m_btnBackpack->setStyleSheet(navBtnStyle);
    m_btnBackpack->setCursor(Qt::PointingHandCursor);

    bottomRow->addWidget(m_btnBigMap);
    bottomRow->addWidget(m_btnSmallMap);
    bottomRow->addWidget(m_btnBackpack);
    bottomRow->addStretch();

    m_btnGameMenu = new QPushButton(QStringLiteral("游戏菜单"), this);
    m_btnGameMenu->setStyleSheet(QStringLiteral(
        "QPushButton{background-color:#e94560;color:#ffffff;border:none;"
        "border-radius:6px;font-size:14px;padding:8px 20px;min-width:100px}"
        "QPushButton:hover{background-color:#ff6b81}"));
    m_btnGameMenu->setCursor(Qt::PointingHandCursor);
    bottomRow->addWidget(m_btnGameMenu);

    outer->addLayout(bottomRow);

    // ---- Connections ----
    connect(m_btnBigMap,   &QPushButton::clicked, this, &GamePage::openBigMap);
    connect(m_btnSmallMap, &QPushButton::clicked, this, &GamePage::openSmallMap);
    connect(m_btnBackpack, &QPushButton::clicked, this, &GamePage::openBackpack);
    connect(m_btnGameMenu, &QPushButton::clicked, this, &GamePage::openGameMenu);

    // Initial refresh
    refreshTileContext();
}

// ---------------------------------------------------------------------------
// Tile / building context
// ---------------------------------------------------------------------------
void GamePage::refreshTileContext()
{
    const MapTile *tile = m_map->currentTile();
    if (!tile) return;

    if (!m_inBuilding) {
        m_currentElements = tile->elements;
    }
    refreshElementTable();
    refreshLocationInfo();
    refreshStats();
}

void GamePage::enterBuilding(int buildingId)
{
    // Find the building on the current tile
    const MapTile *tile = m_map->currentTile();
    if (!tile) return;

    m_inBuilding = true;
    m_buildingId = buildingId;
    m_buildingTileX = m_map->playerX();
    m_buildingTileY = m_map->playerY();

    // Get or generate building interior
    m_currentElements = m_map->buildingInterior(
        m_buildingTileX, m_buildingTileY, buildingId);

    // Find building name
    for (const auto &e : tile->elements) {
        if (e.type == MapElementType::SmallHouse && e.id == buildingId) {
            m_buildingName = QStringLiteral("%1 #%2").arg(e.name()).arg(buildingId);
            break;
        }
    }

    refreshElementTable();
    refreshLocationInfo();
    refreshStats();
}

void GamePage::leaveBuilding()
{
    m_inBuilding = false;
    m_buildingId = 0;
    m_buildingName.clear();
    refreshTileContext();
}

void GamePage::containerOpened(int buildingId, int containerId)
{
    // Validate: if indoor, must match current building
    if (buildingId > 0 && (!m_inBuilding || m_buildingId != buildingId)) return;
    for (auto &e : m_currentElements) {
        if (e.isContainer() && e.id == containerId) {
            e.containerOpened = true;
            break;
        }
    }
    refreshElementTable();
}

// ---------------------------------------------------------------------------
// Element table refresh
// ---------------------------------------------------------------------------
void GamePage::refreshElementTable()
{
    m_elemTable->setRowCount(0);

    const QString btnStyle = QStringLiteral(
        "QPushButton{background-color:#16213e;color:#c0c0d0;"
        "border:1px solid #0f3460;border-radius:4px;"
        "font-size:13px;padding:4px 12px}"
        "QPushButton:hover{background-color:#0f3460;color:#fff;border-color:#e94560}");

    int row = 0;
    for (const auto &elem : m_currentElements) {
        m_elemTable->insertRow(row);

        // Column 0: name (with ID for buildings & containers)
        QString displayName = elem.name();
        if (elem.isBuilding() || elem.isContainer())
            displayName = QStringLiteral("%1 #%2").arg(elem.name()).arg(elem.id);
        auto *nameItem = new QTableWidgetItem(displayName);
        nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
        m_elemTable->setItem(row, 0, nameItem);

        // Column 1: action button
        QPushButton *btn = nullptr;
        if (elem.canGather()) {
            int mins = elem.gatherTimeMinutes();
            btn = new QPushButton(
                QStringLiteral("%1 %2min").arg(elem.actionLabel()).arg(mins), this);
        } else if (elem.isBuilding()) {
            btn = new QPushButton(elem.actionLabel(), this);
        } else if (elem.isContainer()) {
            btn = new QPushButton(elem.actionLabel(), this);
        }
        if (btn) {
            btn->setStyleSheet(btnStyle);
            btn->setCursor(Qt::PointingHandCursor);
            int r = row;
            connect(btn, &QPushButton::clicked, this, [this, r]() { onElementAction(r); });
            m_elemTable->setCellWidget(row, 1, btn);
        }

        ++row;
    }

    // If in building, add "离开建筑" row
    if (m_inBuilding) {
        m_elemTable->insertRow(row);
        auto *leaveItem = new QTableWidgetItem(QStringLiteral("离开建筑"));
        leaveItem->setFlags(leaveItem->flags() & ~Qt::ItemIsEditable);
        m_elemTable->setItem(row, 0, leaveItem);

        auto *leaveBtn = new QPushButton(QStringLiteral("离开"), this);
        leaveBtn->setStyleSheet(btnStyle);
        leaveBtn->setCursor(Qt::PointingHandCursor);
        connect(leaveBtn, &QPushButton::clicked, this, [this]() {
            leaveBuilding();
            emit leaveBuildingRequested();
        });
        m_elemTable->setCellWidget(row, 1, leaveBtn);
    }

    m_elemTable->resizeRowsToContents();
}

// ---------------------------------------------------------------------------
// Element action handler
// ---------------------------------------------------------------------------
void GamePage::onElementAction(int row)
{
    if (row < 0 || row >= static_cast<int>(m_currentElements.size())) return;
    const TileElement &elem = m_currentElements[row];

    if (elem.canGather()) {
        applyGather(elem);
    } else if (elem.isBuilding()) {
        emit enterBuildingRequested(elem, m_map->playerX(), m_map->playerY());
    } else if (elem.isContainer()) {
        int bid = m_inBuilding ? m_buildingId : 0;
        emit openContainerRequested(elem, m_map->playerX(), m_map->playerY(), bid);
    }
}

// ---------------------------------------------------------------------------
// Gather logic
// ---------------------------------------------------------------------------
// =============================================================================
// Gather logic — the primary action: cut trees / collect stones / search bins
// =============================================================================
// Flow:
//   1. Check if inventory has space for at least one guaranteed loot item
//   2. Process hourly ticks for the action duration
//   3. Apply stat changes (hunger/thirst cost for the labour)
//   4. Roll bonus loot via RNG
//   5. Add items to inventory (tracking full-bag state)
//   6. Advance game time, remove the resource from the map (outdoor only)
//   7. Check auto-save interval, then refresh the UI
void GamePage::applyGather(const TileElement &elem)
{
    // Check if inventory has enough space for at least the guaranteed loot
    auto loot = elementGatherLoot(elem.type);
    bool hasSpace = false;
    for (const auto &entry : loot) {
        Item probe = makeLootItem(entry.itemName);
        if (m_inventory->canAdd(probe)) {
            hasSpace = true;
            break;
        }
    }
    if (!hasSpace) {
        m_invFullLabel->setVisible(true);
        QTimer::singleShot(3000, m_invFullLabel, [this]() { m_invFullLabel->setVisible(false); });
        return;
    }

    int mins = elem.gatherTimeMinutes();

    // Save pre-tick day/hour for auto-save
    int oldDay = m_time.day();

    // Process hourly ticks
    processHourlyTicks(mins);

    // Gather cost (moderate)
    m_stats.applyChanges({
        {Stat::Hunger, -10},
        {Stat::Thirst, -5}
    });

    // Generate loot — roll dice and add items to inventory
    std::vector<Item> rolled;
    rollGatherLoot(elem.type, rolled);
    bool invFull = false;
    for (auto &item : rolled) {
        if (!m_inventory->addItem(item))
            invFull = true;
    }

    if (invFull) {
        m_invFullLabel->setVisible(true);
        QTimer::singleShot(3000, m_invFullLabel, [this]() { m_invFullLabel->setVisible(false); });
    }

    // Advance time
    m_time.advance(mins);

    // Remove element from tile
    if (!m_inBuilding) {
        m_map->gatherElement(elem.id);
    }

    // Auto-save check
    if (m_autoSaveInterval > 0) {
        int oldCycle = (oldDay - 1) / m_autoSaveInterval;
        int newCycle = (m_time.day() - 1) / m_autoSaveInterval;
        if (oldCycle < newCycle)
            emit autoSaveTriggered();
    }

    refreshTileContext();
}

// ---------------------------------------------------------------------------
// Location info display
// ---------------------------------------------------------------------------
void GamePage::refreshLocationInfo()
{
    m_timeLabel->setText(m_time.displayString());

    const MapTile *tile = m_map->currentTile();
    if (tile) {
        m_tileLabel->setText(QStringLiteral("%1  (%2, %3)")
            .arg(tile->typeName())
            .arg(m_map->playerX())
            .arg(m_map->playerY()));

        if (m_inBuilding) {
            m_locLabel->setText(m_buildingName);
        } else {
            m_locLabel->setText(QStringLiteral("户外"));
        }
    }
}

// ---------------------------------------------------------------------------
// Stats
// ---------------------------------------------------------------------------
void GamePage::refreshStats()
{
    QLabel *labels[] = { m_hpLabel, m_hungerLabel, m_thirstLabel, m_sanityLabel, m_restLabel };
    for (int i = 0; i < kStatCount; ++i)
        labels[i]->setText(m_stats.statString(static_cast<Stat>(i)));
}

void GamePage::applyItemEffects(const std::vector<StatChange> &effects)
{
    m_stats.applyChanges(effects);
    refreshStats();
}

double GamePage::statValue(Stat t) const
{
    return m_stats.value(t);
}

// =============================================================================
// Process in-game hours: apply hourly stat ticks for each hour passed
// =============================================================================
void GamePage::processHourlyTicks(int minutesPassed)
{
    int hours = minutesPassed / 60;
    for (int i = 0; i < hours; ++i)
        m_stats.applyHourlyTick();
}

// ---------------------------------------------------------------------------
// Test items
// ---------------------------------------------------------------------------
void GamePage::setupTestItems()
{
    m_inventory->addItem(Item(QStringLiteral("面包"),
        "一块干硬的面包", 0.1,
        {{Stat::Hunger, 10}, {Stat::Thirst, -5}}, true));
    m_inventory->addItem(Item(QStringLiteral("木板"),
        "一块厚实的木板", 0.5));
    m_inventory->addItem(Item(QStringLiteral("石子"),
        "一块粗糙的石头", 2.0));
}

// =============================================================================
// Full game state reset — called when starting a new game from main menu
// =============================================================================
void GamePage::resetGame()
{
    m_stats = PlayerStats();
    m_time = GameTime();
    m_inventory->clear();
    m_inBuilding = false;
    m_buildingId = 0;
    m_buildingName.clear();
    m_currentElements.clear();
    m_autoSaveInterval = 1;
    m_map->reset();
    setupTestItems();
    refreshTileContext();
}
