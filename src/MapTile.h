#ifndef MAPTILE_H
#define MAPTILE_H

#include <QString>
#include <QStringList>
#include <vector>

struct MapTile
{
    enum Type { Plain, Mountain, TypeCount };
    enum State { Unknown, Explored, Visible };

    Type type = Plain;
    int x = 0;
    int y = 0;
    double moveDifficulty = 1.0;
    State state = Unknown;

    QString typeName() const {
        switch (type) {
        case Plain:    return QStringLiteral("平地");
        case Mountain: return QStringLiteral("山地");
        default:       return {};
        }
    }

    QString typeDesc() const {
        switch (type) {
        case Plain:    return QStringLiteral("平坦的开阔地，移动难度低");
        case Mountain: return QStringLiteral("崎岖的山地，移动难度高");
        default:       return {};
        }
    }

    std::vector<QString> elements; // small-map elements (trees, rocks, etc.)
};

#endif // MAPTILE_H
