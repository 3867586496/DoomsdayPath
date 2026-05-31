#ifndef MAPELEMENT_H
#define MAPELEMENT_H

#include <QString>
#include <vector>

// Element types on map tiles / inside buildings
enum class MapElementType {
    None,
    Tree,        // 大树 — gatherable natural resource
    Stone,       // 石头 — gatherable natural resource
    SmallHouse,  // 小平房 — building (enterable)
    TrashCan     // 垃圾桶 — container (inside buildings)
};

struct TileElement {
    MapElementType type = MapElementType::None;
    int id = 0;              // unique numbering per type, 1-based
    bool containerOpened = false;

    QString name() const;
    QString actionLabel() const;  // e.g. "砍伐", "采集", "进入", "打开", "搜索"

    bool canGather() const   { return type == MapElementType::Tree || type == MapElementType::Stone; }
    bool isBuilding() const  { return type == MapElementType::SmallHouse; }
    bool isContainer() const { return type == MapElementType::TrashCan; }

    int gatherTimeMinutes() const  { return 60; }
    int searchTimeMinutes() const  { return 2; }

    bool operator==(const TileElement &o) const {
        return type == o.type && id == o.id;
    }
};

// Loot entry for gathering / container searching
struct GatherLoot {
    QString itemName;
    int guaranteed = 0;
    double bonusChance = 0.0;
    int bonusMin = 0;
    int bonusMax = 0;
};

std::vector<GatherLoot> elementGatherLoot(MapElementType type);

#endif // MAPELEMENT_H
