#ifndef WORLDMAP_H
#define WORLDMAP_H

#include "MapTile.h"
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

public slots:
    void generateInitial();

signals:
    void mapChanged();
    void playerMoved(int x, int y);

private:
    void updateFog();
    MapTile &ensureTile(int x, int y);

    using Key = std::pair<int, int>;
    std::map<Key, MapTile> m_tiles;
    int m_playerX = 0;
    int m_playerY = 0;
};

#endif // WORLDMAP_H
