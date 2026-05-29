#include "BackpackPage.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

BackpackPage::BackpackPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void BackpackPage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 16, 20, 16);

    // 閳ユ柡鈧?閺嶅洭顣?閳ユ柡鈧?    QLabel *titleLabel = new QLabel(QStringLiteral("閼冲苯瀵?), this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(QStringLiteral(
        "QLabel {"
        "  color: #e94560;"
        "  font-size: 28px;"
        "  font-weight: bold;"
        "  padding: 8px;"
        "}"
    ));
    mainLayout->addWidget(titleLabel);

    // 閳ユ柡鈧?鐠愮喖鍣搁弰鍓с仛 閳ユ柡鈧?    m_weightLabel = new QLabel(this);
    m_weightLabel->setAlignment(Qt::AlignCenter);
    m_weightLabel->setStyleSheet(QStringLiteral(
        "QLabel {"
        "  color: #a0a0b0;"
        "  font-size: 18px;"
        "  padding: 6px;"
        "}"
    ));
    mainLayout->addWidget(m_weightLabel);

    mainLayout->addSpacing(8);

    // 閳ユ柡鈧?閻椻晛鎼ч崚妤勩€?閳ユ柡鈧?    m_table = new QTableWidget(0, 3, this);
    m_table->setHorizontalHeaderLabels({
        QStringLiteral("閸氬秶袨"),
        QStringLiteral("閸旂喕鍏?),
        QStringLiteral("閹垮秳缍?)
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

    m_table->setStyleSheet(QStringLiteral(
        "QTableWidget {"
        "  background-color: #1a1a2e;"
        "  border: 1px solid #0f3460;"
        "  gridline-color: #0f3460;"
        "  color: #e0e0e0;"
        "  font-size: 15px;"
        "}"
        "QTableWidget::item {"
        "  padding: 6px 12px;"
        "}"
        "QTableWidget::item:alternate {"
        "  background-color: #162040;"
        "}"
        "QHeaderView::section {"
        "  background-color: #0f3460;"
        "  color: #a0a0b0;"
        "  font-size: 14px;"
        "  font-weight: bold;"
        "  padding: 6px 12px;"
        "  border: none;"
        "  border-bottom: 2px solid #1a1a2e;"
        "}"
    ));
    mainLayout->addWidget(m_table);

    mainLayout->addSpacing(16);

    // 閳ユ柡鈧?閸忔娊妫撮幐澶愭尦 閳ユ柡鈧?    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();

    m_btnClose = new QPushButton(QStringLiteral("閸忔娊妫撮懗灞藉瘶"), this);
    m_btnClose->setStyleSheet(QStringLiteral(
        "QPushButton {"
        "  background-color: #16213e;"
        "  color: #e0e0e0;"
        "  border: 2px solid #0f3460;"
        "  border-radius: 8px;"
        "  font-size: 16px;"
        "  padding: 8px 24px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #0f3460;"
        "  border-color: #e94560;"
        "}"
    ));
    m_btnClose->setCursor(Qt::PointingHandCursor);
    connect(m_btnClose, &QPushButton::clicked, this, &BackpackPage::closed);
    bottomLayout->addWidget(m_btnClose);

    mainLayout->addLayout(bottomLayout);

    refresh();
}

void BackpackPage::setInventory(Inventory *inventory)
{
    if (m_inventory) {
        disconnect(m_inventory, &Inventory::changed, this, &BackpackPage::refresh);
    }
    m_inventory = inventory;
    if (m_inventory) {
        connect(m_inventory, &Inventory::changed, this, &BackpackPage::refresh);
    }
    refresh();
}

void BackpackPage::refresh()
{
    if (!m_inventory) {
        m_weightLabel->setText(QStringLiteral("鐠愮喖鍣? 閳?閳?));
        m_table->setRowCount(0);
        return;
    }

    m_weightLabel->setText(QStringLiteral("鐠愮喖鍣? %1 / %2")
        .arg(m_inventory->currentWeight(), 0, 'f', 1)
        .arg(m_inventory->capacity(), 0, 'f', 1));

    m_table->setRowCount(0);
    m_table->setRowCount(m_inventory->count());

    const QString cellStyle = QStringLiteral(
        "QLabel { color: #e0e0e0; font-size: 15px; padding: 4px 8px; }"
    );
    const QString actionBtnStyle = QStringLiteral(
        "QPushButton {"
        "  background-color: #16213e;"
        "  color: #c0c0d0;"
        "  border: 1px solid #0f3460;"
        "  border-radius: 4px;"
        "  font-size: 13px;"
        "  padding: 3px 12px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #1a2744;"
        "  border-color: #e94560;"
        "  color: #ffffff;"
        "}"
    );

    for (int i = 0; i < m_inventory->count(); ++i) {
        const Item *item = m_inventory->itemAt(i);
        if (!item) continue;

        // 缁楊兛绔撮崚妤嬬窗閸氬秶袨
        QLabel *nameLabel = new QLabel(item->name());
        nameLabel->setStyleSheet(cellStyle);
        m_table->setCellWidget(i, 0, nameLabel);

        // 缁楊兛绨╅崚妤嬬窗閸旂喕鍏?        QLabel *funcLabel = new QLabel(item->functionText());
        funcLabel->setStyleSheet(cellStyle);
        m_table->setCellWidget(i, 1, funcLabel);

        // 缁楊兛绗侀崚妤嬬窗閹垮秳缍旈幐澶愭尦
        QWidget *actionWidget = new QWidget();
        QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);
        actionLayout->setContentsMargins(4, 2, 4, 2);
        actionLayout->setSpacing(6);

        if (item->isEdible()) {
            QPushButton *btnEat = new QPushButton(QStringLiteral("妞嬬喓鏁?));
            btnEat->setStyleSheet(actionBtnStyle);
            btnEat->setCursor(Qt::PointingHandCursor);
            int row = i;
            connect(btnEat, &QPushButton::clicked, this, [this, row]() {
                onUseItem(row);
            });
            actionLayout->addWidget(btnEat);
        }

        QPushButton *btnDiscard = new QPushButton(QStringLiteral("娑撱垹绱?));
        btnDiscard->setStyleSheet(actionBtnStyle);
        btnDiscard->setCursor(Qt::PointingHandCursor);
        int row = i;
        connect(btnDiscard, &QPushButton::clicked, this, [this, row]() {
            onDiscardItem(row);
        });
        actionLayout->addWidget(btnDiscard);

        m_table->setCellWidget(i, 2, actionWidget);
    }
}

void BackpackPage::onDiscardItem(int row)
{
    if (m_inventory) {
        m_inventory->removeItem(row);
    }
}

void BackpackPage::onUseItem(int row)
{
    if (!m_inventory) return;
    const Item *item = m_inventory->itemAt(row);
    if (!item || !item->isEdible()) return;

    std::vector<StatChange> effects = item->effects();
    m_inventory->removeItem(row);
    emit itemUsed(effects);
    emit closed();
}
