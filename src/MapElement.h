#ifndef MAPELEMENT_H
#define MAPELEMENT_H

#include <QString>
#include <vector>

// Required by makeLootItem
struct Item;
Item makeLootItem(const QString &lootName);

// =============================================================================
// Element type registry — add a row to introduce a new interactable object
// =============================================================================
enum class MapElementType : int {
    None       = 0,
    Tree       = 1,  // gatherable natural resource
    Stone      = 2,  // gatherable natural resource
    SmallHouse = 3,  // building (enterable)
    TrashCan   = 4,  // container (searchable)
};

struct ElementTypeInfo {
    const char *name;
    const char *ungatheredAction;  // label before first interaction
    const char *gatheredAction;    // label after (containers: "打开" after search)
    bool gatherable;
    bool isBuilding;
    bool isContainer;
    int gatherMinutes;
    int searchMinutes;
};

// Ordered to match MapElementType enum values (index 0 = None, skip)
constexpr ElementTypeInfo kElementTypes[] = {
    // name          ungatheredAct  gatheredAct   canGather bldg  cntnr  gatherM  searchM
    { "",            "",            "",            false, false, false, 0,  0 },  // None
    { "大树",        "砍伐",        "砍伐",         true,  false, false, 60, 0 },  // Tree
    { "石头",        "采集",        "采集",         true,  false, false, 60, 0 },  // Stone
    { "小平房",      "进入",        "进入",         false, true,  false, 0,  0 },  // SmallHouse
    { "垃圾桶",      "搜索",        "打开",         false, false, true,  0,  2 },  // TrashCan
};
constexpr int kElementTypeCount = sizeof(kElementTypes) / sizeof(kElementTypes[0]);

inline const ElementTypeInfo &elementInfo(MapElementType t) {
    int idx = static_cast<int>(t);
    if (idx < 0 || idx >= kElementTypeCount) return kElementTypes[0];
    return kElementTypes[idx];
}

struct TileElement {
    MapElementType type = MapElementType::None;
    int id = 0;              // unique numbering per type, 1-based
    bool containerOpened = false;

    QString name() const;
    QString actionLabel() const;  // "砍伐", "采集", "进入", "搜索"/"打开"

    bool canGather() const   { return elementInfo(type).gatherable; }
    bool isBuilding() const  { return elementInfo(type).isBuilding; }
    bool isContainer() const { return elementInfo(type).isContainer; }

    int gatherTimeMinutes() const { return elementInfo(type).gatherMinutes; }
    int searchTimeMinutes() const { return elementInfo(type).searchMinutes; }

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
