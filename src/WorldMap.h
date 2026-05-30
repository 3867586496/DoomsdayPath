#ifndef WORLDMAP_H
#define WORLDMAP_H

#include "MapTile.h"
#include <QObject>
#include <QRandomGenerator>
#include <vector>

class WorldMap : public QObject
{
    Q_OBJECT

public:
    static constexpr int MapSize = 20;  // total grid is 20x20
    static constexpr int FogRadius = 1; // 3x3 visible around player

    explicit WorldMap(QObject *parent = nullptr);

    int playerX() const { return m_playerX; }
    int playerY() const { return m_playerY; }

    const MapTile *tileAt(int x, int y) const;
    MapTile *tileAt(int x, int y);

    // Time cost to move from one tile to adjacent tile (in minutes)
    int moveCostBetween(int x1, int y1, int x2, int y2) const;

    // Move player: updates fog, returns true if move was valid
    bool movePlayer(int dx, int dy);

    // Total grid size
    int size() const { return MapSize; }

public slots:
    void generateRandom();

signals:
    void mapChanged();
    void playerMoved(int x, int y);

private:
    void updateFog();
    bool isValidPos(int x, int y) const;

    std::vector<std::vector<MapTile>> m_tiles;
    int m_playerX = MapSize / 2;
    int m_playerY = MapSize / 2;
};

#endif // WORLDMAP_H
