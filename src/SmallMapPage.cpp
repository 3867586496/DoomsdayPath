#include "SmallMapPage.h"
#include "WorldMap.h"
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

SmallMapPage::SmallMapPage(WorldMap *map, QWidget *parent)
    : QWidget(parent), m_map(map)
{
    setupUI();
}

void SmallMapPage::setupUI()
{
    QVBoxLayout *ml = new QVBoxLayout(this);
    ml->setContentsMargins(12, 12, 12, 12);

    QLabel *title = new QLabel(QStringLiteral("小地图"), this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(QStringLiteral(
        "QLabel{color:#e94560;font-size:22px;font-weight:bold;padding:8px}"));
    ml->addWidget(title);

    m_posLabel = new QLabel(this);
    m_posLabel->setAlignment(Qt::AlignCenter);
    m_posLabel->setStyleSheet(QStringLiteral(
        "QLabel{color:#a0a0b0;font-size:16px;padding:4px}"));
    ml->addWidget(m_posLabel);
    ml->addSpacing(8);

    m_table = new QTableWidget(0, 2, this);
    m_table->setHorizontalHeaderLabels({
        QStringLiteral("元素"), QStringLiteral("数量")
    });
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->verticalHeader()->setVisible(false);
    m_table->verticalHeader()->setDefaultSectionSize(48);
    m_table->setShowGrid(true);
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    m_table->horizontalHeader()->resizeSection(1, 80);
    m_table->setStyleSheet(QStringLiteral(
        "QTableWidget{background-color:#1a1a2e;border:1px solid #0f3460;"
        "gridline-color:#0f3460;color:#e0e0e0;font-size:15px}"
        "QTableWidget::item{padding:10px 12px}"
        "QHeaderView::section{background-color:#0f3460;color:#a0a0b0;"
        "font-size:14px;font-weight:bold;padding:8px 12px;"
        "border:none;border-bottom:2px solid #1a1a2e}"));
    ml->addWidget(m_table);
    ml->addSpacing(16);

    QHBoxLayout *bl = new QHBoxLayout();
    bl->addStretch();
    QPushButton *btnBack = new QPushButton(QStringLiteral("返回"), this);
    btnBack->setStyleSheet(QStringLiteral(
        "QPushButton{background-color:#16213e;color:#e0e0e0;"
        "border:2px solid #0f3460;border-radius:8px;"
        "font-size:16px;padding:8px 24px}"
        "QPushButton:hover{background-color:#0f3460;border-color:#e94560}"));
    btnBack->setCursor(Qt::PointingHandCursor);
    connect(btnBack, &QPushButton::clicked, this, &SmallMapPage::returnToGame);
    bl->addWidget(btnBack);
    ml->addLayout(bl);

    refresh();
}

void SmallMapPage::refresh()
{
    if (!m_map) return;
    int px = m_map->playerX(), py = m_map->playerY();
    auto *tile = m_map->tileAt(px, py);

    m_posLabel->setText(QStringLiteral("当前位置: (%1, %2) %3")
        .arg(px).arg(py)
        .arg(tile && tile->state != MapTile::Unknown ? tile->typeName() : "???"));

    if (!tile || tile->state == MapTile::Unknown) {
        m_table->setRowCount(0);
        return;
    }

    // Count elements by name
    QMap<QString, int> counts;
    for (const auto &e : tile->elements)
        counts[e.name()]++;

    m_table->setRowCount(counts.size());
    int row = 0;
    QString cellStyle = QStringLiteral(
        "QLabel{color:#e0e0e0;font-size:14px;padding:4px 8px}");
    for (auto it = counts.begin(); it != counts.end(); ++it, ++row) {
        QLabel *nl = new QLabel(it.key());
        nl->setStyleSheet(cellStyle);
        m_table->setCellWidget(row, 0, nl);

        QLabel *ql = new QLabel(QString::number(it.value()));
        ql->setAlignment(Qt::AlignCenter);
        ql->setStyleSheet(cellStyle);
        m_table->setCellWidget(row, 1, ql);

        m_table->setRowHeight(row, 48);
    }
}
