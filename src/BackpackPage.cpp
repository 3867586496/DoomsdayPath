#include "BackpackPage.h"
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

BackpackPage::BackpackPage(QWidget *parent) : QWidget(parent) { setupUI(); }

void BackpackPage::setupUI() {
    QVBoxLayout *ml = new QVBoxLayout(this);
    ml->setContentsMargins(20, 16, 20, 16);

    // Title
    QLabel *tl = new QLabel(QStringLiteral("Backpack"), this);
    tl->setAlignment(Qt::AlignCenter);
    tl->setStyleSheet(QStringLiteral(
        "QLabel{color:#e94560;font-size:28px;font-weight:bold;padding:8px}"));
    ml->addWidget(tl);

    // Weight display
    m_weightLabel = new QLabel(this);
    m_weightLabel->setAlignment(Qt::AlignCenter);
    m_weightLabel->setStyleSheet(QStringLiteral(
        "QLabel{color:#a0a0b0;font-size:18px;padding:6px}"));
    ml->addWidget(m_weightLabel);
    ml->addSpacing(8);

    // Table: Name | Qty | Function | Action
    m_table = new QTableWidget(0, 4, this);
    m_table->setHorizontalHeaderLabels({
        QStringLiteral("Name"),
        QStringLiteral("Qty"),
        QStringLiteral("Function"),
        QStringLiteral("Action")
    });
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->verticalHeader()->setVisible(false);
    m_table->setShowGrid(false);
    m_table->setAlternatingRowColors(true);
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_table->setStyleSheet(QStringLiteral(
        "QTableWidget{background-color:#1a1a2e;border:1px solid #0f3460;"
        "gridline-color:#0f3460;color:#e0e0e0;font-size:15px}"
        "QTableWidget::item{padding:6px 12px}"
        "QTableWidget::item:alternate{background-color:#162040}"
        "QHeaderView::section{background-color:#0f3460;color:#a0a0b0;"
        "font-size:14px;font-weight:bold;padding:6px 12px;"
        "border:none;border-bottom:2px solid #1a1a2e}"));
    ml->addWidget(m_table);
    ml->addSpacing(16);

    // Close button
    QHBoxLayout *bl = new QHBoxLayout();
    bl->addStretch();
    m_btnClose = new QPushButton(QStringLiteral("Close Backpack"), this);
    m_btnClose->setStyleSheet(QStringLiteral(
        "QPushButton{background-color:#16213e;color:#e0e0e0;"
        "border:2px solid #0f3460;border-radius:8px;"
        "font-size:16px;padding:8px 24px}"
        "QPushButton:hover{background-color:#0f3460;border-color:#e94560}"));
    m_btnClose->setCursor(Qt::PointingHandCursor);
    connect(m_btnClose, &QPushButton::clicked, this, &BackpackPage::closed);
    bl->addWidget(m_btnClose);
    ml->addLayout(bl);

    refresh();
}

void BackpackPage::setInventory(Inventory *inv) {
    if (m_inventory)
        disconnect(m_inventory, &Inventory::changed, this, &BackpackPage::refresh);
    m_inventory = inv;
    if (m_inventory)
        connect(m_inventory, &Inventory::changed, this, &BackpackPage::refresh);
    refresh();
}

void BackpackPage::refresh() {
    if (!m_inventory) {
        m_weightLabel->setText(QStringLiteral("Weight: -/-"));
        m_table->setRowCount(0);
        return;
    }
    m_weightLabel->setText(QStringLiteral("Weight: %1 / %2")
        .arg(m_inventory->currentWeight(), 0, 'f', 1)
        .arg(m_inventory->capacity(), 0, 'f', 1));

    // Group items by name for quantity display
    struct Row {
        int firstIdx;
        int qty;
    };
    std::vector<Row> rows;
    for (int i = 0; i < m_inventory->count(); ++i) {
        const Item *item = m_inventory->itemAt(i);
        if (!item) continue;
        bool found = false;
        for (auto &r : rows) {
            const Item *first = m_inventory->itemAt(r.firstIdx);
            if (first && first->name() == item->name()) {
                r.qty++;
                found = true;
                break;
            }
        }
        if (!found) rows.push_back({i, 1});
    }

    m_table->setRowCount(0);
    m_table->setRowCount(static_cast<int>(rows.size()));

    QString cellStyle = QStringLiteral(
        "QLabel{color:#e0e0e0;font-size:15px;padding:4px 8px}");
    QString btnStyle = QStringLiteral(
        "QPushButton{background-color:#16213e;color:#c0c0d0;"
        "border:1px solid #0f3460;border-radius:4px;"
        "font-size:13px;padding:3px 12px}"
        "QPushButton:hover{background-color:#1a2744;"
        "border-color:#e94560;color:#fff}");

    for (int row = 0; row < static_cast<int>(rows.size()); ++row) {
        const Item *item = m_inventory->itemAt(rows[row].firstIdx);
        if (!item) continue;
        int firstIdx = rows[row].firstIdx;

        // Col 0: Name
        QLabel *nl = new QLabel(item->name());
        nl->setStyleSheet(cellStyle);
        m_table->setCellWidget(row, 0, nl);

        // Col 1: Quantity
        QLabel *ql = new QLabel(QString::number(rows[row].qty));
        ql->setAlignment(Qt::AlignCenter);
        ql->setStyleSheet(cellStyle);
        m_table->setCellWidget(row, 1, ql);

        // Col 2: Function description
        QString funcDesc;
        if (!item->effects().empty()) {
            QStringList parts;
            for (const auto &e : item->effects()) {
                QString sign = e.amount >= 0 ? QStringLiteral("+") : QString();
                parts.append(QStringLiteral("%1%2%3")
                    .arg(Action::targetName(e.target))
                    .arg(sign)
                    .arg(e.amount, 0, 'f', 0));
            }
            funcDesc = parts.join(QStringLiteral(", "));
        } else {
            funcDesc = QStringLiteral("-");
        }
        QLabel *fl = new QLabel(funcDesc);
        fl->setStyleSheet(cellStyle);
        m_table->setCellWidget(row, 2, fl);

        // Col 3: Action buttons
        QWidget *aw = new QWidget();
        QHBoxLayout *al = new QHBoxLayout(aw);
        al->setContentsMargins(4, 2, 4, 2);
        al->setSpacing(6);

        if (item->isEdible()) {
            QPushButton *be = new QPushButton(QStringLiteral("Eat"));
            be->setStyleSheet(btnStyle);
            be->setCursor(Qt::PointingHandCursor);
            connect(be, &QPushButton::clicked, this,
                    [this, firstIdx]() { onUseItem(firstIdx); });
            al->addWidget(be);
        }

        QPushButton *bd = new QPushButton(QStringLiteral("Discard"));
        bd->setStyleSheet(btnStyle);
        bd->setCursor(Qt::PointingHandCursor);
        connect(bd, &QPushButton::clicked, this,
                [this, firstIdx]() { onDiscardItem(firstIdx); });
        al->addWidget(bd);

        m_table->setCellWidget(row, 3, aw);
    }
}

void BackpackPage::onDiscardItem(int row) {
    if (m_inventory) m_inventory->removeItem(row);
}

void BackpackPage::onUseItem(int row) {
    if (!m_inventory) return;
    const Item *item = m_inventory->itemAt(row);
    if (!item || !item->isEdible()) return;
    std::vector<StatChange> effects = item->effects();
    m_inventory->removeItem(row);
    emit itemUsed(effects);
    // Item consumed, inventory refreshes automatically via changed() signal
}
