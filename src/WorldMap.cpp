#include "WorldMap.h"

WorldMap::WorldMap(QObject *parent) : QObject(parent)
{
    m_tiles.resize(MapSize, std::vector<MapTile>(MapSize));
    for (int y = 0; y < MapSize; ++y)
        for (int x = 0; x < MapSize; ++x) {
            m_tiles[y][x].x = x;
            m_tiles[y][x].y = y;
        }
    generateRandom();
}

const MapTile *WorldMap::tileAt(int x, int y) const
{
    if (!isValidPos(x, y)) return nullptr;
    return &m_tiles[y][x];
}

MapTile *WorldMap::tileAt(int x, int y)
{
    if (!isValidPos(x, y)) return nullptr;
    return &m_tiles[y][x];
}

bool WorldMap::isValidPos(int x, int y) const
{
    return x >= 0 && x < MapSize && y >= 0 && y < MapSize;
}

void WorldMap::generateRandom()
{
    auto *rng = QRandomGenerator::global();
    for (int y = 0; y < MapSize; ++y)
        for (int x = 0; x < MapSize; ++x) {
            auto &t = m_tiles[y][x];
            t.state = MapTile::Unknown;
            t.elements.clear();
            int r = rng->bounded(100);
            if (r < 60) {
                t.type = MapTile::Plain;
                t.moveDifficulty = 1.0;
                int trees = rng->bounded(3, 13);
                for (int i = 0; i < trees; ++i)
                    t.elements.push_back(QStringLiteral("大树"));
            } else {
                t.type = MapTile::Mountain;
                t.moveDifficulty = 2.0;
                int rocks = rng->bounded(3, 13);
                for (int i = 0; i < rocks; ++i)
                    t.elements.push_back(QStringLiteral("石头"));
            }
        }
    updateFog();
    emit mapChanged();
}

int WorldMap::moveCostBetween(int x1, int y1, int x2, int y2) const
{
    auto *a = tileAt(x1, y1);
    auto *b = tileAt(x2, y2);
    if (!a || !b) return 0;
    return static_cast<int>((a->moveDifficulty + b->moveDifficulty) * 30);
}

bool WorldMap::movePlayer(int dx, int dy)
{
    int nx = m_playerX + dx;
    int ny = m_playerY + dy;
    if (!isValidPos(nx, ny)) return false;
    // Only allow orthogonal moves
    if (dx != 0 && dy != 0) return false;
    if (dx == 0 && dy == 0) return false;
    m_playerX = nx;
    m_playerY = ny;
    updateFog();
    emit playerMoved(m_playerX, m_playerY);
    emit mapChanged();
    return true;
}

void WorldMap::updateFog()
{
    // Reveal 3x3 around player
    for (int dy = -FogRadius; dy <= FogRadius; ++dy)
        for (int dx = -FogRadius; dx <= FogRadius; ++dx) {
            int x = m_playerX + dx;
            int y = m_playerY + dy;
            if (isValidPos(x, y))
                m_tiles[y][x].state = MapTile::Visible;
        }
    // Mark current position as explored
    auto *t = tileAt(m_playerX, m_playerY);
    if (t) {
        t->state = MapTile::Explored;
        // Mark adjacent explored tiles
        for (int dy = -1; dy <= 1; ++dy)
            for (int dx = -1; dx <= 1; ++dx) {
                int x = m_playerX + dx;
                int y = m_playerY + dy;
                if (isValidPos(x, y) && m_tiles[y][x].state == MapTile::Unknown)
                    continue;
            }
    }
}
