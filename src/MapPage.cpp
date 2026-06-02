// =============================================================================
// MapPage — 9×9 overhead grid view of the world
// =============================================================================
// Renders a viewport centered on the player. Tiles are pre-created QPushButtons
// stored in m_tiles[9][9] — refreshGrid updates existing widgets instead of
// destroying/recreating, avoiding the 81-button allocation churn on every move.
//
// Terrain colours come from the kTileTypes registry (MapTile.h).

#include "MapPage.h"
#include "WorldMap.h"
#include "MapTile.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QShowEvent>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

// Shared tile stylesheet template
static const QString kTileTemplate = QStringLiteral(
    "QPushButton{background-color:%1;border:2px solid %2;"
    "color:#e0e0e0;font-size:20px;border-radius:4px}"
    "QPushButton:hover{border-color:#e94560}");

MapPage::MapPage(WorldMap *map, QWidget *parent)
    : QWidget(parent), m_map(map)
{
    setupUI();
    setFocusPolicy(Qt::StrongFocus);
    connect(m_map, &WorldMap::mapChanged, this, &MapPage::refreshGrid);
}

void MapPage::setupUI()
{
    auto *ml = new QVBoxLayout(this);
    ml->setContentsMargins(8, 8, 8, 8);

    // Top bar — title + coordinate info
    auto *topBar = new QHBoxLayout();
    auto *title = new QLabel(QStringLiteral("大地图"), this);
    title->setStyleSheet("QLabel{color:#e94560;font-size:22px;font-weight:bold}");
    topBar->addWidget(title);
    topBar->addStretch();
    m_infoLabel = new QLabel(this);
    m_infoLabel->setStyleSheet("QLabel{color:#a0a0b0;font-size:14px}");
    topBar->addWidget(m_infoLabel);
    ml->addLayout(topBar);
    ml->addSpacing(4);

    // Grid — pre-create all widgets for reuse
    auto *gridWidget = new QWidget(this);
    m_gridLayout = new QGridLayout(gridWidget);
    m_gridLayout->setSpacing(2);
    m_gridLayout->setContentsMargins(0, 0, 0, 0);

    // Corner cell (empty)
    m_gridLayout->addWidget(new QLabel(), 0, 0);

    // Column headers (row 0, cols 1–9)
    for (int c = 0; c < 9; ++c) {
        m_colHeaders[c] = new QLabel(this);
        m_colHeaders[c]->setAlignment(Qt::AlignCenter);
        m_colHeaders[c]->setStyleSheet("QLabel{color:#666680;font-size:11px;padding:0}");
        m_colHeaders[c]->setFixedWidth(CellSize);
        m_gridLayout->addWidget(m_colHeaders[c], 0, c + 1);
    }

    // Row headers + tile buttons
    for (int r = 0; r < 9; ++r) {
        m_rowHeaders[r] = new QLabel(this);
        m_rowHeaders[r]->setAlignment(Qt::AlignCenter);
        m_rowHeaders[r]->setStyleSheet("QLabel{color:#666680;font-size:11px;padding:0}");
        m_rowHeaders[r]->setFixedHeight(CellSize);
        m_gridLayout->addWidget(m_rowHeaders[r], r + 1, 0);

        for (int c = 0; c < 9; ++c) {
            m_tiles[r][c] = new QPushButton(this);
            m_tiles[r][c]->setFixedSize(CellSize, CellSize);
            m_tiles[r][c]->setCursor(Qt::PointingHandCursor);
            m_gridLayout->addWidget(m_tiles[r][c], r + 1, c + 1);
        }
    }

    ml->addWidget(gridWidget, 1);

    // Bottom bar
    auto *botBar = new QHBoxLayout();
    auto *hint = new QLabel(QStringLiteral("WASD/方向键移动 | 点击相邻地块移动"), this);
    hint->setStyleSheet("QLabel{color:#666680;font-size:13px}");
    botBar->addWidget(hint);
    botBar->addStretch();

    auto *btnBack = new QPushButton(QStringLiteral("返回"), this);
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
    int px = m_map->playerX();
    int py = m_map->playerY();

    // Update column headers
    for (int c = 0; c < 9; ++c)
        m_colHeaders[c]->setText(QString::number(px - ViewRadius + c));

    // Update row headers + tile buttons
    for (int r = 0; r < 9; ++r) {
        m_rowHeaders[r]->setText(QString::number(py - ViewRadius + r));

        for (int c = 0; c < 9; ++c) {
            int mx = px - ViewRadius + c;
            int my = py - ViewRadius + r;
            updateTileButton(c, r, mx, my);
        }
    }

    // Info label
    auto *ct = m_map->tileAt(px, py);
    if (ct && ct->state != MapTile::Unknown)
        m_infoLabel->setText(QStringLiteral("(%1, %2) %3  难度: %4")
            .arg(px).arg(py).arg(ct->typeName()).arg(ct->moveDifficulty, 0, 'f', 1));
}

void MapPage::updateTileButton(int col, int row, int mapX, int mapY)
{
    auto *btn = m_tiles[row][col];
    auto *tile = m_map->tileAt(mapX, mapY);

    int px = m_map->playerX();
    int py = m_map->playerY();
    bool isPlayer = (mapX == px && mapY == py);
    bool isAdjacent = (abs(mapX - px) + abs(mapY - py) == 1);

    QString bg, border, text;

    if (isPlayer) {
        bg = "#e94560"; border = "#ff6b81"; text = QStringLiteral("📍");
    } else if (!tile || tile->state == MapTile::Unknown) {
        bg = "#0a0a1a"; border = "#111130"; text = "?";
    } else {
        auto &info = tileTypeInfo(tile->type);
        bg = QString::fromUtf8(info.bgHex);
        border = QString::fromUtf8(info.borderHex);
        text = QString::fromUtf8(info.icon);
    }

    btn->setText(text);
    btn->setStyleSheet(kTileTemplate.arg(bg, border));

    btn->disconnect();  // clear old click connections (adjacency changes on move)
    if (isAdjacent && tile && tile->state != MapTile::Unknown) {
        int cost = m_map->moveCostBetween(px, py, mapX, mapY);
        btn->setToolTip(QStringLiteral("%1 (%2, %3)\n难度: %4\n移动耗时: %5分钟\n%6")
            .arg(tile->typeName()).arg(mapX).arg(mapY)
            .arg(tile->moveDifficulty, 0, 'f', 1).arg(cost).arg(tile->typeDesc()));
        connect(btn, &QPushButton::clicked, this, [this, mapX, mapY]() {
            int dx = mapX - m_map->playerX();
            int dy = mapY - m_map->playerY();
            tryMove(dx, dy);
        });
    } else {
        btn->setToolTip({});
    }
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
