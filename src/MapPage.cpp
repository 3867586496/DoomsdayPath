#include "MapPage.h"
#include "WorldMap.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QShowEvent>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

MapPage::MapPage(WorldMap *map, QWidget *parent)
    : QWidget(parent), m_map(map)
{
    setupUI();
    setFocusPolicy(Qt::StrongFocus);
    connect(m_map, &WorldMap::mapChanged, this, &MapPage::refreshGrid);
}

void MapPage::setupUI()
{
    QVBoxLayout *ml = new QVBoxLayout(this);
    ml->setContentsMargins(8, 8, 8, 8);

    QHBoxLayout *topBar = new QHBoxLayout();
    QLabel *title = new QLabel(QStringLiteral("大地图"), this);
    title->setStyleSheet(QStringLiteral(
        "QLabel{color:#e94560;font-size:22px;font-weight:bold}"));
    topBar->addWidget(title);
    topBar->addStretch();

    m_infoLabel = new QLabel(this);
    m_infoLabel->setStyleSheet(QStringLiteral(
        "QLabel{color:#a0a0b0;font-size:14px}"));
    topBar->addWidget(m_infoLabel);
    ml->addLayout(topBar);
    ml->addSpacing(4);

    // Grid
    QWidget *gridWidget = new QWidget(this);
    m_gridLayout = new QGridLayout(gridWidget);
    m_gridLayout->setSpacing(2);
    m_gridLayout->setContentsMargins(0, 0, 0, 0);
    ml->addWidget(gridWidget, 1);

    // Bottom bar
    QHBoxLayout *botBar = new QHBoxLayout();
    QLabel *hint = new QLabel(QStringLiteral("WASD/方向键移动 | 点击相邻地块移动"), this);
    hint->setStyleSheet(QStringLiteral("QLabel{color:#666680;font-size:13px}"));
    botBar->addWidget(hint);
    botBar->addStretch();

    QPushButton *btnBack = new QPushButton(QStringLiteral("返回"), this);
    btnBack->setStyleSheet(QStringLiteral(
        "QPushButton{background-color:#16213e;color:#e0e0e0;"
        "border:2px solid #0f3460;border-radius:8px;"
        "font-size:16px;padding:8px 24px}"
        "QPushButton:hover{background-color:#0f3460;border-color:#e94560}"));
    btnBack->setCursor(Qt::PointingHandCursor);
    connect(btnBack, &QPushButton::clicked, this, &MapPage::returnToGame);
    botBar->addWidget(btnBack);
    ml->addLayout(botBar);

    refreshGrid();
}

void MapPage::refreshGrid()
{
    // Clear existing grid
    QLayoutItem *item;
    while ((item = m_gridLayout->takeAt(0)) != nullptr) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    int px = m_map->playerX();
    int py = m_map->playerY();

    // Column headers
    m_gridLayout->addWidget(new QLabel(""), 0, 0);
    for (int dx = -ViewRadius; dx <= ViewRadius; ++dx) {
        int mx = px + dx;
        QLabel *colLabel = new QLabel(QString::number(mx));
        colLabel->setAlignment(Qt::AlignCenter);
        colLabel->setStyleSheet(QStringLiteral(
            "QLabel{color:#666680;font-size:11px;padding:0}"));
        colLabel->setFixedWidth(CellSize);
        m_gridLayout->addWidget(colLabel, 0, dx + ViewRadius + 1);
    }

    for (int dy = -ViewRadius; dy <= ViewRadius; ++dy) {
        int my = py + dy;
        // Row header
        QLabel *rowLabel = new QLabel(QString::number(my));
        rowLabel->setAlignment(Qt::AlignCenter);
        rowLabel->setStyleSheet(QStringLiteral(
            "QLabel{color:#666680;font-size:11px;padding:0}"));
        rowLabel->setFixedHeight(CellSize);
        m_gridLayout->addWidget(rowLabel, dy + ViewRadius + 1, 0);

        for (int dx = -ViewRadius; dx <= ViewRadius; ++dx) {
            int mx = px + dx;
            int my = py + dy;
            auto *w = createTileWidget(mx, my);
            w->setFixedSize(CellSize, CellSize);
            m_gridLayout->addWidget(w, dy + ViewRadius + 1, dx + ViewRadius + 1);
        }
    }

    // Update info
    auto *ct = m_map->tileAt(px, py);
    if (ct && ct->state != MapTile::Unknown)
        m_infoLabel->setText(QStringLiteral("(%1, %2) %3  难度: %4")
            .arg(px).arg(py).arg(ct->typeName()).arg(ct->moveDifficulty, 0, 'f', 1));
}

QWidget *MapPage::createTileWidget(int mapX, int mapY)
{
    auto *tile = m_map->tileAt(mapX, mapY);
    QPushButton *btn = new QPushButton(this);

    int px = m_map->playerX();
    int py = m_map->playerY();

    bool isPlayer = (mapX == px && mapY == py);
    bool isAdjacent = (abs(mapX - px) + abs(mapY - py) == 1);

    QString bg, border, text;

    if (isPlayer) {
        bg = "#e94560";
        border = "#ff6b81";
        text = QStringLiteral("📍");
    } else if (!tile || tile->state == MapTile::Unknown) {
        bg = "#0a0a1a";
        border = "#111130";
        text = "?";
    } else {
        if (tile->type == MapTile::Plain) {
            bg = "#1a3a1a";
            border = "#2a5a2a";
            text = QStringLiteral("平");
        } else if (tile->type == MapTile::Village) {
            bg = "#2a4a1a";
            border = "#4a7a2a";
            text = QStringLiteral("村");
        } else {
            bg = "#3a2a1a";
            border = "#5a3a2a";
            text = QStringLiteral("山");
        }
    }

    btn->setText(text);
    btn->setStyleSheet(QStringLiteral(
        "QPushButton{background-color:%1;border:2px solid %2;"
        "color:#e0e0e0;font-size:20px;border-radius:4px}"
        "QPushButton:hover{border-color:#e94560}").arg(bg, border));
    btn->setCursor(Qt::PointingHandCursor);

    if (isAdjacent && tile && tile->state != MapTile::Unknown) {
        int cost = m_map->moveCostBetween(px, py, mapX, mapY);
        btn->setToolTip(QStringLiteral("%1 (%2, %3)\n难度: %4\n移动耗时: %5分钟\n%6")
            .arg(tile->typeName())
            .arg(mapX).arg(mapY)
            .arg(tile->moveDifficulty, 0, 'f', 1)
            .arg(cost)
            .arg(tile->typeDesc()));
        connect(btn, &QPushButton::clicked, this, [this, mapX, mapY]() {
            int dx = mapX - m_map->playerX();
            int dy = mapY - m_map->playerY();
            tryMove(dx, dy);
        });
    }

    return btn;
}

void MapPage::tryMove(int dx, int dy)
{
    int nx = m_map->playerX() + dx;
    int ny = m_map->playerY() + dy;
    int cost = m_map->moveCostBetween(m_map->playerX(), m_map->playerY(), nx, ny);
    if (m_map->movePlayer(dx, dy)) {
        emit tileEntered(nx, ny, cost);
        refreshGrid();
    }
}

void MapPage::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_W:     case Qt::Key_Up:    tryMove(0, -1); break;
    case Qt::Key_S:     case Qt::Key_Down:  tryMove(0, 1);  break;
    case Qt::Key_A:     case Qt::Key_Left:  tryMove(-1, 0); break;
    case Qt::Key_D:     case Qt::Key_Right: tryMove(1, 0);  break;
    default: QWidget::keyPressEvent(event); return;
    }
}

void MapPage::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    setFocus();
}
