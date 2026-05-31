#include "MapElement.h"
#include "Item.h"

// =============================================================================
// MapElement — TileElement helpers and loot tables
// =============================================================================

QString TileElement::name() const
{
    return QString::fromUtf8(elementInfo(type).name);
}

QString TileElement::actionLabel() const
{
    auto &info = elementInfo(type);
    if (isContainer())
        return containerOpened
            ? QString::fromUtf8(info.gatheredAction)
            : QString::fromUtf8(info.ungatheredAction);
    return QString::fromUtf8(info.ungatheredAction);
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

Item makeLootItem(const QString &lootName)
{
    if (lootName == QStringLiteral("木板"))
        return Item(lootName, QStringLiteral("一块厚实的木板"), 0.5);
    if (lootName == QStringLiteral("石子"))
        return Item(lootName, QStringLiteral("一块粗糙的石头"), 2.0);
    if (lootName == QStringLiteral("废纸"))
        return Item(lootName, QStringLiteral("一张废纸"), 0.01);
    return Item(lootName, {}, 0);
}
