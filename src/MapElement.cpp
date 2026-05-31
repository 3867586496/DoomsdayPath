#include "MapElement.h"

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
