#ifndef MAPTILE_H
#define MAPTILE_H

#include <QString>
#include <vector>
#include "MapElement.h"

// =============================================================================
// Tile type registry — add a row here to introduce a new terrain
// =============================================================================
// Each type defines: icon character, display name, description,
// default difficulty, and a generation weight (for ensureTile roulette).
struct TileTypeInfo {
    const char *key;
    const char *name;
    const char *desc;
    const char *icon;       // single CJK char used on the big map
    const char *bgHex;      // background colour #RRGGBB
    const char *borderHex;  // border colour #RRGGBB
    double defaultDifficulty;
    int generationWeight;   // relative weight in the spawn roulette (total ~100)
};

// Ordered by generation priority — ensureTile rolls in this sequence
constexpr TileTypeInfo kTileTypes[] = {
    // key      name     desc                       icon  bg       border    diff  weight
    { "Plain",   "平地", "平坦的开阔地，移动难度低",        "平", "#1a3a1a", "#2a5a2a", 1.0,  34 },
    { "Mountain","山地", "崎岖的山地，移动难度高",        "山", "#3a2a1a", "#5a3a2a", 2.0,  33 },
    { "Village", "农村", "废弃的农村，可能有建筑",        "村", "#2a4a1a", "#4a7a2a", 1.0,  33 },
};
constexpr int kTileTypeCount = sizeof(kTileTypes) / sizeof(kTileTypes[0]);

// Lookup helpers (return defaults if type out of range)
inline const TileTypeInfo &tileTypeInfo(int type) {
    if (type < 0 || type >= kTileTypeCount) return kTileTypes[0];
    return kTileTypes[type];
}

struct MapTile
{
    enum Type : int {
        Plain    = 0,
        Mountain = 1,
        Village  = 2,
    };
    enum State { Unknown, Explored, Visible };

    Type type = Plain;
    int x = 0;
    int y = 0;
    double moveDifficulty = 1.0;
    State state = Unknown;

    QString typeName() const  { return QString::fromUtf8(tileTypeInfo(type).name); }
    QString typeDesc() const  { return QString::fromUtf8(tileTypeInfo(type).desc); }
    QString typeIcon() const  { return QString::fromUtf8(tileTypeInfo(type).icon); }

    std::vector<TileElement> elements;
};

#endif // MAPTILE_H
