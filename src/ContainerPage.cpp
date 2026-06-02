#include "ContainerPage.h"
#include "MapElement.h"
#include "style.h"

// =============================================================================
// ContainerPage — Backpack ↔ Container transfer UI
// =============================================================================

#include <functional>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <functional>

// ---- Static helpers ---------------------------------------------------------

// Group identical items by name; returns (firstIndex, quantity) per group
std::vector<ContainerPage::MergeRow>
ContainerPage::mergeByName(const std::vector<Item> &items)
{
    std::vector<MergeRow> merged;
    for (int i = 0; i < static_cast<int>(items.size()); ++i) {
        bool found = false;
        for (auto &r : merged) {
            if (items[r.firstIdx].name() == items[i].name()) {
                r.qty++;
                found = true;
                break;
            }
        }
        if (!found)
            merged.push_back({i, 1});
    }
    return merged;
}

// Shared transfer button style (used in both tables)
static const QString kBtnStyle = QStringLiteral(
    "QPushButton{background-color:%1;color:%2;"
    "border:1px solid %3;border-radius:6px;font-size:13px;padding:4px 12px}"
    "QPushButton:hover{background-color:%1;color:white;border-color:%4}")
    .arg(Style::kPanelBg, Style::kText, Style::kBorder, Style::kAccent);

// Shared tables style
static const QString kTableStyle = QStringLiteral(
    "QTableWidget{background-color:%1;color:%2;"
    "gridline-color:%3;border:1px solid %3;border-radius:8px;font-size:13px}"
    "QHeaderView::section{background-color:%1;color:%4;"
    "border:none;padding:6px;font-weight:bold}")
    .arg(Style::kPanelBg, Style::kText, Style::kBorder, Style::kAccent);

// ---- Construction -----------------------------------------------------------

ContainerPage::ContainerPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

// Setup a pair of 4-column tables (name | qty | fn | action) with headers
static QTableWidget *makeTransferTable(const QString &title, QWidget *parent,
                                       QBoxLayout *tablesRow)
{
    auto *box = new QVBoxLayout();
    auto *label = new QLabel(title, parent);
    label->setStyleSheet("QLabel{color:#c0c0d0;font-size:14px;font-weight:bold}");
    label->setAlignment(Qt::AlignCenter);
    box->addWidget(label);

    auto *table = new QTableWidget(0, 4, parent);
    table->setHorizontalHeaderLabels({QStringLiteral("名称"), QStringLiteral("数量"),
                                      QStringLiteral("功能"), QStringLiteral("操作")});
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    table->verticalHeader()->setVisible(false);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setFocusPolicy(Qt::NoFocus);
    table->setStyleSheet(kTableStyle);
    box->addWidget(table);
    tablesRow->addLayout(box);
    return table;
}

void ContainerPage::setupUI()
{
    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(12, 8, 12, 8);

    m_titleLabel = new QLabel(this);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet(QStringLiteral(
        "QLabel{color:%1;font-size:22px;font-weight:bold;padding:8px}")
        .arg(Style::kAccent));
    outer->addWidget(m_titleLabel);

    m_btnSearch = new QPushButton(QStringLiteral("搜索容器"), this);
    m_btnSearch->setStyleSheet(QStringLiteral(
        "QPushButton{background-color:%1;color:white;border:none;"
        "border-radius:8px;font-size:16px;padding:10px 30px}"
        "QPushButton:hover{background-color:%2}")
        .arg(Style::kAccent, Style::kAccentH));
    m_btnSearch->setCursor(Qt::PointingHandCursor);
    m_btnSearch->setVisible(false);
    outer->addWidget(m_btnSearch, 0, Qt::AlignCenter);

    auto *tablesRow = new QHBoxLayout();
    m_backpackTable  = makeTransferTable(QStringLiteral("背包"), this, tablesRow);
    m_containerTable = makeTransferTable(QStringLiteral("容器"), this, tablesRow);
    outer->addLayout(tablesRow, 1);

    m_btnReturn = new QPushButton(QStringLiteral("返回"), this);
    m_btnReturn->setStyleSheet(QStringLiteral(
        "QPushButton{background-color:%1;color:%2;"
        "border:1px solid %3;border-radius:8px;"
        "font-size:16px;padding:10px 40px}"
        "QPushButton:hover{background-color:%1;color:white;border-color:%4}")
        .arg(Style::kPanelBg, Style::kText, Style::kBorder, Style::kAccent));
    m_btnReturn->setCursor(Qt::PointingHandCursor);
    outer->addWidget(m_btnReturn, 0, Qt::AlignCenter);

    // ---- Connections ----
    connect(m_btnReturn, &QPushButton::clicked, this, &ContainerPage::closed);

    connect(m_btnSearch, &QPushButton::clicked, this, [this]() {
        m_btnSearch->setVisible(false);
        rollGatherLoot(m_container.type, m_containerItems);
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

    m_titleLabel->setText(QStringLiteral("%1 #%2").arg(container.name()).arg(container.id));

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

// ---- Populate tables --------------------------------------------------------

// Build one merged table (name | qty | fn | action-button)
static void fillMergedTable(QTableWidget *table,
                            const std::vector<Item> &items,
                            const QString &btnLabel,
                            QObject *owner,
                            std::function<void(int)> onTransfer)
{
    table->setRowCount(0);
    auto merged = ContainerPage::mergeByName(items);

    for (int r = 0; r < static_cast<int>(merged.size()); ++r) {
        const auto &row = merged[r];
        const Item &item = items[row.firstIdx];
        table->insertRow(r);

        table->setItem(r, 0, new QTableWidgetItem(item.name()));
        table->setItem(r, 1, new QTableWidgetItem(QString::number(row.qty)));

        QString func = item.functionText();
        if (func.isEmpty()) func = QStringLiteral("-");
        table->setItem(r, 2, new QTableWidgetItem(func));

        auto *btn = new QPushButton(btnLabel, table);
        btn->setStyleSheet(kBtnStyle);
        btn->setCursor(Qt::PointingHandCursor);
        int groupIdx = r;
        QObject::connect(btn, &QPushButton::clicked, owner, [onTransfer, groupIdx]() {
            onTransfer(groupIdx);
        });
        table->setCellWidget(r, 3, btn);
    }
}

void ContainerPage::populateContainerItems()
{
    fillMergedTable(m_containerTable, m_containerItems,
                    QStringLiteral("← 取出"), this,
                    [this](int g) { onTransferToBackpack(g); });
}

void ContainerPage::populateBackpackItems()
{
    fillMergedTable(m_backpackTable, m_backpackItems,
                    QStringLiteral("存入 →"), this,
                    [this](int g) { onTransferToContainer(g); });
}

// ---- Transfer ---------------------------------------------------------------

void ContainerPage::onTransferToBackpack(int mergedRow)
{
    auto merged = mergeByName(m_containerItems);
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
    auto merged = mergeByName(m_backpackItems);
    if (mergedRow < 0 || mergedRow >= static_cast<int>(merged.size())) return;
    int idx = merged[mergedRow].firstIdx;
    Item item = m_backpackItems[idx];

    emit itemTransferredToContainer(item, 1);
    m_backpackItems.erase(m_backpackItems.begin() + idx);
    m_containerItems.push_back(item);
    refresh();
}
