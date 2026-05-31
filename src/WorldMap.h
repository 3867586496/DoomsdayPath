#ifndef WORLDMAP_H
#define WORLDMAP_H

#include "MapTile.h"
#include "MapElement.h"
#include <QObject>
#include <QRandomGenerator>
#include <map>
#include <utility>

class WorldMap : public QObject
{
    Q_OBJECT

public:
    static constexpr int FogRadius = 1;     // 3×3 visible around player
    static constexpr int SpawnRadius = 5;   // initial generation around spawn

    explicit WorldMap(QObject *parent = nullptr);

    int playerX() const { return m_playerX; }
    int playerY() const { return m_playerY; }

    const MapTile *tileAt(int x, int y) const;
    MapTile *tileAt(int x, int y);

    // Time cost in minutes to move between two adjacent tiles
    int moveCostBetween(int x1, int y1, int x2, int y2) const;

    // Move player: updates fog, returns true if valid
    bool movePlayer(int dx, int dy);

    // Building interiors — generated on first access
    std::vector<TileElement> buildingInterior(int tileX, int tileY, int buildingId);

    // Current tile convenience
    const MapTile *currentTile() const { return tileAt(m_playerX, m_playerY); }

    // Gather a natural resource from current tile (returns true if found & removed)
    bool gatherElement(int elementId);

public slots:
    void generateInitial();

signals:
    void mapChanged();
    void playerMoved(int x, int y);

private:
    void updateFog();
    MapTile &ensureTile(int x, int y);

    using Key = std::pair<int, int>;
    using InteriorKey = std::tuple<int, int, int>;  // tileX, tileY, buildingId
    std::map<Key, MapTile> m_tiles;
    std::map<InteriorKey, std::vector<TileElement>> m_buildingInteriors;
    int m_playerX = 0;
    int m_playerY = 0;
};

#endif // WORLDMAP_H
