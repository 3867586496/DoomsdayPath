#ifndef ITEM_H
#define ITEM_H

#include <QString>
#include <vector>
#include "StatChange.h"

class Item
{
public:
    Item() = default;
    Item(const QString &name, const QString &description,
         double weight, const std::vector<StatChange> &effects = {},
         bool edible = false);

    QString name() const { return m_name; }
    QString description() const { return m_description; }
    double weight() const { return m_weight; }
    const std::vector<StatChange> &effects() const { return m_effects; }
    bool isEdible() const { return m_edible; }

    // Function description for backpack list column 2
    QString functionText() const;

private:
    QString m_name;
    QString m_description;
    double m_weight = 0.0;
    std::vector<StatChange> m_effects;
    bool m_edible = false;
};

#endif // ITEM_H
