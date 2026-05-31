#include "MapElement.h"

// =============================================================================
// MapElement — TileElement helpers and loot tables
// =============================================================================
// TileElement is the fundamental unit on a map tile or inside a building.
// Each has a type, a per-type sequential id, and interaction state
// (containerOpened / gathered).
//
// Action labels:
//   Tree       → "砍伐"  60min   guaranteed 3 木板 + 50% bonus 1-3
//   Stone      → "采集"  60min   guaranteed 3 石子 + 50% bonus 1-3
//   SmallHouse → "进入"  (opens building interior)
//   TrashCan   → "搜索"  2min (unopened) / "打开" (opened) 50% 废纸 ×1

QString TileElement::name() const
{
    switch (type) {
    case MapElementType::Tree:       return QStringLiteral("大树");
    case MapElementType::Stone:      return QStringLiteral("石头");
    case MapElementType::SmallHouse: return QStringLiteral("小平房");
    case MapElementType::TrashCan:   return QStringLiteral("垃圾桶");
    default:                         return {};
    }
}

QString TileElement::actionLabel() const
{
    switch (type) {
    case MapElementType::Tree:       return QStringLiteral("砍伐");
    case MapElementType::Stone:      return QStringLiteral("采集");
    case MapElementType::SmallHouse: return QStringLiteral("进入");
    case MapElementType::TrashCan:
        return containerOpened ? QStringLiteral("打开") : QStringLiteral("搜索");
    default: return {};
    }
}

std::vector<GatherLoot> elementGatherLoot(MapElementType type)
{
    switch (type) {
    case MapElementType::Tree:
        return {
            {QStringLiteral("木板"), 3, 50.0, 1, 3}
        };
    case MapElementType::Stone:
        return {
            {QStringLiteral("石子"), 3, 50.0, 1, 3}
        };
    case MapElementType::TrashCan:
        return {
            {QStringLiteral("废纸"), 0, 50.0, 1, 1}
        };
    default:
        return {};
    }
}
