#include "ContainerPage.h"
#include "MapElement.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QRandomGenerator>
#include <QTableWidget>
#include <QVBoxLayout>

ContainerPage::ContainerPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void ContainerPage::setupUI()
{
    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(12, 8, 12, 8);

    // Title
    m_titleLabel = new QLabel(this);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet(QStringLiteral(
        "QLabel{color:#e94560;font-size:22px;font-weight:bold;padding:8px}"));
    outer->addWidget(m_titleLabel);

    // Search button (for unopened containers)
    m_btnSearch = new QPushButton(QStringLiteral("搜索容器"), this);
    m_btnSearch->setStyleSheet(QStringLiteral(
        "QPushButton{background-color:#e94560;color:#fff;border:none;"
        "border-radius:6px;font-size:16px;padding:10px 30px}"
        "QPushButton:hover{background-color:#ff6b81}"));
    m_btnSearch->setCursor(Qt::PointingHandCursor);
    m_btnSearch->setVisible(false);
    outer->addWidget(m_btnSearch, 0, Qt::AlignCenter);

    // ---- Tables row ----
    auto *tablesRow = new QHBoxLayout();
    const QString tableStyle = QStringLiteral(
        "QTableWidget{background-color:#0f0f1a;color:#e0e0e0;"
        "gridline-color:#1a1a2e;border:1px solid #1a1a2e;font-size:13px}"
        "QHeaderView::section{background-color:#16213e;color:#c0c0d0;"
        "border:none;padding:4px;font-weight:bold}");

    // Backpack table (left)
    auto *bpBox = new QVBoxLayout();
    auto *bpLabel = new QLabel(QStringLiteral("背包"), this);
    bpLabel->setStyleSheet("QLabel{color:#c0c0d0;font-size:14px;font-weight:bold}");
    bpLabel->setAlignment(Qt::AlignCenter);
    bpBox->addWidget(bpLabel);

    m_backpackTable = new QTableWidget(0, 4, this);
    m_backpackTable->setHorizontalHeaderLabels(
        {QStringLiteral("名称"), QStringLiteral("数量"), QStringLiteral("功能"), QStringLiteral("操作")});
    m_backpackTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_backpackTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_backpackTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_backpackTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_backpackTable->verticalHeader()->setVisible(false);
    m_backpackTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_backpackTable->setSelectionMode(QAbstractItemView::NoSelection);
    m_backpackTable->setFocusPolicy(Qt::NoFocus);
    m_backpackTable->setStyleSheet(tableStyle);
    bpBox->addWidget(m_backpackTable);
    tablesRow->addLayout(bpBox);

    // Container table (right)
    auto *ctBox = new QVBoxLayout();
    auto *ctLabel = new QLabel(QStringLiteral("容器"), this);
    ctLabel->setStyleSheet("QLabel{color:#c0c0d0;font-size:14px;font-weight:bold}");
    ctLabel->setAlignment(Qt::AlignCenter);
    ctBox->addWidget(ctLabel);

    m_containerTable = new QTableWidget(0, 4, this);
    m_containerTable->setHorizontalHeaderLabels(
        {QStringLiteral("名称"), QStringLiteral("数量"), QStringLiteral("功能"), QStringLiteral("操作")});
    m_containerTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_containerTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_containerTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_containerTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_containerTable->verticalHeader()->setVisible(false);
    m_containerTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_containerTable->setSelectionMode(QAbstractItemView::NoSelection);
    m_containerTable->setFocusPolicy(Qt::NoFocus);
    m_containerTable->setStyleSheet(tableStyle);
    ctBox->addWidget(m_containerTable);
    tablesRow->addLayout(ctBox);

    outer->addLayout(tablesRow, 1);

    // Return button
    m_btnReturn = new QPushButton(QStringLiteral("返回"), this);
    m_btnReturn->setStyleSheet(QStringLiteral(
        "QPushButton{background-color:#16213e;color:#c0c0d0;"
        "border:1px solid #0f3460;border-radius:6px;"
        "font-size:16px;padding:10px 40px}"
        "QPushButton:hover{background-color:#0f3460;color:#fff;border-color:#e94560}"));
    m_btnReturn->setCursor(Qt::PointingHandCursor);
    outer->addWidget(m_btnReturn, 0, Qt::AlignCenter);

    // Connections
    connect(m_btnReturn, &QPushButton::clicked, this, &ContainerPage::closed);

    connect(m_btnSearch, &QPushButton::clicked, this, [this]() {
        m_btnSearch->setVisible(false);  // prevent double-tap loot duplication
        // Generate loot for unopened container
        auto loot = elementGatherLoot(m_container.type);
        auto *rng = QRandomGenerator::global();
        for (const auto &entry : loot) {
            int qty = entry.guaranteed;
            if (entry.bonusChance > 0 && rng->bounded(100) < entry.bonusChance) {
                qty += rng->bounded(entry.bonusMin, entry.bonusMax + 1);
            }
            for (int i = 0; i < qty; ++i) {
                m_containerItems.push_back(
                    Item(QStringLiteral("废纸"), QStringLiteral("一张废纸"), 0.01));
            }
        }
        emit containerSearched(m_containerTileX, m_containerTileY,
                               m_buildingId, m_container.id);
        refresh();
    });
}

void ContainerPage::setContainer(const TileElement &container,
                                  int tileX, int tileY, int buildingId)
{
    m_container = container;
    m_containerTileX = tileX;
    m_containerTileY = tileY;
    m_buildingId = buildingId;
    m_containerItems.clear();

    m_titleLabel->setText(QStringLiteral("%1 #%2")
        .arg(container.name()).arg(container.id));

    // If container was already opened, hide search; if not, show it
    if (!container.containerOpened) {
        m_btnSearch->setVisible(true);
        m_btnSearch->setText(QStringLiteral("搜索 (%1分钟)")
            .arg(container.searchTimeMinutes()));
    } else {
        m_btnSearch->setVisible(false);
    }

    refresh();
}

void ContainerPage::setBackpackInventory(const std::vector<Item> &items)
{
    m_backpackItems = items;
    refresh();
}

void ContainerPage::setContainerItems(const std::vector<Item> &items)
{
    m_containerItems = items;
    refresh();
}

void ContainerPage::refresh()
{
    populateContainerItems();
    populateBackpackItems();
}

void ContainerPage::populateContainerItems()
{
    m_containerTable->setRowCount(0);

    // Merge identical items
    struct Row { int firstIdx; int qty; };
    std::vector<Row> merged;
    for (int i = 0; i < static_cast<int>(m_containerItems.size()); ++i) {
        bool found = false;
        for (auto &r : merged) {
            if (m_containerItems[r.firstIdx].name() == m_containerItems[i].name()) {
                r.qty++;
                found = true;
                break;
            }
        }
        if (!found)
            merged.push_back({i, 1});
    }

    const QString btnStyle = QStringLiteral(
        "QPushButton{background-color:#16213e;color:#c0c0d0;"
        "border:1px solid #0f3460;border-radius:4px;font-size:13px;padding:4px 10px}"
        "QPushButton:hover{background-color:#0f3460;color:#fff;border-color:#e94560}");

    for (int r = 0; r < static_cast<int>(merged.size()); ++r) {
        const auto &row = merged[r];
        const Item &item = m_containerItems[row.firstIdx];
        m_containerTable->insertRow(r);

        m_containerTable->setItem(r, 0,
            new QTableWidgetItem(item.name()));
        m_containerTable->setItem(r, 1,
            new QTableWidgetItem(QString::number(row.qty)));

        // Function text
        QString func = item.functionText();
        if (func.isEmpty()) func = QStringLiteral("-");
        m_containerTable->setItem(r, 2, new QTableWidgetItem(func));

        // Transfer to backpack button
        auto *btn = new QPushButton(QStringLiteral("← 取出"), this);
        btn->setStyleSheet(btnStyle);
        btn->setCursor(Qt::PointingHandCursor);
        int idx = r;
        connect(btn, &QPushButton::clicked, this, [this, idx]() {
            onTransferToBackpack(idx);
        });
        m_containerTable->setCellWidget(r, 3, btn);
    }
}

void ContainerPage::populateBackpackItems()
{
    m_backpackTable->setRowCount(0);

    struct Row { int firstIdx; int qty; };
    std::vector<Row> merged;
    for (int i = 0; i < static_cast<int>(m_backpackItems.size()); ++i) {
        bool found = false;
        for (auto &r : merged) {
            if (m_backpackItems[r.firstIdx].name() == m_backpackItems[i].name()) {
                r.qty++;
                found = true;
                break;
            }
        }
        if (!found)
            merged.push_back({i, 1});
    }

    const QString btnStyle = QStringLiteral(
        "QPushButton{background-color:#16213e;color:#c0c0d0;"
        "border:1px solid #0f3460;border-radius:4px;font-size:13px;padding:4px 10px}"
        "QPushButton:hover{background-color:#0f3460;color:#fff;border-color:#e94560}");

    for (int r = 0; r < static_cast<int>(merged.size()); ++r) {
        const auto &row = merged[r];
        const Item &item = m_backpackItems[row.firstIdx];
        m_backpackTable->insertRow(r);

        m_backpackTable->setItem(r, 0,
            new QTableWidgetItem(item.name()));
        m_backpackTable->setItem(r, 1,
            new QTableWidgetItem(QString::number(row.qty)));

        QString func = item.functionText();
        if (func.isEmpty()) func = QStringLiteral("-");
        m_backpackTable->setItem(r, 2, new QTableWidgetItem(func));

        auto *btn = new QPushButton(QStringLiteral("存入 →"), this);
        btn->setStyleSheet(btnStyle);
        btn->setCursor(Qt::PointingHandCursor);
        int idx = r;
        connect(btn, &QPushButton::clicked, this, [this, idx]() {
            onTransferToContainer(idx);
        });
        m_backpackTable->setCellWidget(r, 3, btn);
    }
}

void ContainerPage::onTransferToBackpack(int mergedRow)
{
    // Find first item of this merged group and remove it
    struct Row { int firstIdx; int qty; };
    std::vector<Row> merged;
    for (int i = 0; i < static_cast<int>(m_containerItems.size()); ++i) {
        bool found = false;
        for (auto &r : merged) {
            if (m_containerItems[r.firstIdx].name() == m_containerItems[i].name()) {
                r.qty++;
                found = true;
                break;
            }
        }
        if (!found) merged.push_back({i, 1});
    }
    if (mergedRow < 0 || mergedRow >= static_cast<int>(merged.size())) return;
    int idx = merged[mergedRow].firstIdx;
    Item item = m_containerItems[idx];

    emit itemTransferredToBackpack(item, 1);
    m_containerItems.erase(m_containerItems.begin() + idx);
    m_backpackItems.push_back(item);
    refresh();
}

void ContainerPage::onTransferToContainer(int mergedRow)
{
    struct Row { int firstIdx; int qty; };
    std::vector<Row> merged;
    for (int i = 0; i < static_cast<int>(m_backpackItems.size()); ++i) {
        bool found = false;
        for (auto &r : merged) {
            if (m_backpackItems[r.firstIdx].name() == m_backpackItems[i].name()) {
                r.qty++;
                found = true;
                break;
            }
        }
        if (!found) merged.push_back({i, 1});
    }
    if (mergedRow < 0 || mergedRow >= static_cast<int>(merged.size())) return;
    int idx = merged[mergedRow].firstIdx;
    Item item = m_backpackItems[idx];

    emit itemTransferredToContainer(item, 1);
    m_backpackItems.erase(m_backpackItems.begin() + idx);
    m_containerItems.push_back(item);
    refresh();
}
