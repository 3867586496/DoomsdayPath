#ifndef ACTION_H
#define ACTION_H

#include "StatChange.h"
#include "Item.h"

#include <QString>
#include <vector>

struct LootEntry
{
    Item item;
    int probability; // 0-100
};

class Action
{
public:
    Action() = default;
    Action(const QString &name, const QString &desc,
           const std::vector<StatChange> &costs = {},
           const std::vector<StatChange> &yields = {},
           int timeCostMinutes = 0,
           const std::vector<StatChange> &conditions = {},
           const std::vector<LootEntry> &loot = {});

    QString name() const { return m_name; }
    QString description() const { return m_description; }
    const std::vector<StatChange> &costs() const { return m_costs; }
    const std::vector<StatChange> &yields() const { return m_yields; }
    const std::vector<StatChange> &conditions() const { return m_conditions; }
    const std::vector<LootEntry> &loot() const { return m_loot; }
    int timeCostMinutes() const { return m_timeCost; }

    static QString targetName(StatChange::Target t);

private:
    QString m_name;
    QString m_description;
    std::vector<StatChange> m_costs;
    std::vector<StatChange> m_yields;
    std::vector<StatChange> m_conditions;
    std::vector<LootEntry> m_loot;
    int m_timeCost = 0;
};

#endif // ACTION_H
