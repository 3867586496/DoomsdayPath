#include "WorldMap.h"

WorldMap::WorldMap(QObject *parent) : QObject(parent)
{
    generateInitial();
}

MapTile &WorldMap::ensureTile(int x, int y)
{
    Key k{x, y};
    auto it = m_tiles.find(k);
    if (it != m_tiles.end()) return it->second;

    // Generate new tile on-demand
    MapTile t;
    t.x = x;
    t.y = y;
    t.state = MapTile::Unknown;

    auto *rng = QRandomGenerator::global();
    int r = rng->bounded(100);
    if (r < 34) {
        // 平原: 大树 3-12
        t.type = MapTile::Plain;
        t.moveDifficulty = 1.0;
        int trees = rng->bounded(3, 13);
        for (int i = 0; i < trees; ++i)
            t.elements.push_back({MapElementType::Tree, i + 1});
    } else if (r < 67) {
        // 山地: 石头 3-12
        t.type = MapTile::Mountain;
        t.moveDifficulty = 2.0;
        int rocks = rng->bounded(3, 13);
        for (int i = 0; i < rocks; ++i)
            t.elements.push_back({MapElementType::Stone, i + 1});
    } else {
        // 农村: 小平房 1-3, 大树 3-12, 垃圾桶 1
        t.type = MapTile::Village;
        t.moveDifficulty = 1.0;
        int houses = rng->bounded(1, 4);
        for (int i = 0; i < houses; ++i)
            t.elements.push_back({MapElementType::SmallHouse, i + 1});
        int trees = rng->bounded(3, 13);
        for (int i = 0; i < trees; ++i)
            t.elements.push_back({MapElementType::Tree, i + 1});
        t.elements.push_back({MapElementType::TrashCan, 1});
    }

    m_tiles[k] = t;
    return m_tiles[k];
}

const MapTile *WorldMap::tileAt(int x, int y) const
{
    auto it = m_tiles.find({x, y});
    return it != m_tiles.end() ? &it->second : nullptr;
}

MapTile *WorldMap::tileAt(int x, int y)
{
    auto it = m_tiles.find({x, y});
    return it != m_tiles.end() ? &it->second : nullptr;
}

void WorldMap::generateInitial()
{
    // Pre-generate spawn area
    for (int dy = -SpawnRadius; dy <= SpawnRadius; ++dy)
        for (int dx = -SpawnRadius; dx <= SpawnRadius; ++dx)
            ensureTile(dx, dy);

    // Also generate one ring beyond so adjacent tiles exist
    for (int dy = -SpawnRadius - 1; dy <= SpawnRadius + 1; ++dy) {
        ensureTile(-SpawnRadius - 1, dy);
        ensureTile(SpawnRadius + 1, dy);
    }
    for (int dx = -SpawnRadius; dx <= SpawnRadius; ++dx) {
        ensureTile(dx, -SpawnRadius - 1);
        ensureTile(dx, SpawnRadius + 1);
    }

    // Set all initial tiles as Unknown, then reveal when player is placed
    updateFog();
    emit mapChanged();
}

int WorldMap::moveCostBetween(int x1, int y1, int x2, int y2) const
{
    auto *a = tileAt(x1, y1);
    if (!a) return 60; // default: 1.0 * 2 * 30
    auto *b = tileAt(x2, y2);
    double db = b ? b->moveDifficulty : 1.0;
    return static_cast<int>((a->moveDifficulty + db) * 30);
}

bool WorldMap::movePlayer(int dx, int dy)
{
    // Only orthogonal moves
    if ((dx != 0 && dy != 0) || (dx == 0 && dy == 0)) return false;

    int nx = m_playerX + dx;
    int ny = m_playerY + dy;

    // Ensure target tile exists (generate if needed)
    ensureTile(nx, ny);
    // Pre-generate surrounding tiles for the new location
    for (int fy = ny - 1; fy <= ny + 1; ++fy)
        for (int fx = nx - 1; fx <= nx + 1; ++fx)
            ensureTile(fx, fy);

    m_playerX = nx;
    m_playerY = ny;
    updateFog();
    emit playerMoved(m_playerX, m_playerY);
    emit mapChanged();
    return true;
}

void WorldMap::updateFog()
{
    // Reveal 3×3 around player
    for (int dy = -FogRadius; dy <= FogRadius; ++dy)
        for (int dx = -FogRadius; dx <= FogRadius; ++dx) {
            auto &t = ensureTile(m_playerX + dx, m_playerY + dy);
            t.state = MapTile::Visible;
        }
}

std::vector<TileElement> WorldMap::buildingInterior(int tileX, int tileY, int buildingId)
{
    InteriorKey key{tileX, tileY, buildingId};
    auto it = m_buildingInteriors.find(key);
    if (it != m_buildingInteriors.end())
        return it->second;

    std::vector<TileElement> interior;
    // 小平房: 100% 有垃圾桶
    TileElement trashCan{MapElementType::TrashCan, 1};
    interior.push_back(trashCan);

    m_buildingInteriors[key] = interior;
    return interior;
}

bool WorldMap::gatherElement(int elementId)
{
    auto it = m_tiles.find({m_playerX, m_playerY});
    if (it == m_tiles.end()) return false;
    auto &elements = it->second.elements;
    for (auto git = elements.begin(); git != elements.end(); ++git) {
        if (git->id == elementId && git->canGather()) {
            elements.erase(git);
            emit mapChanged();
            return true;
        }
    }
    return false;
}
