#include "Action.h"

#include <QRandomGenerator>

Action::Action(const QString &name, const QString &desc,
               const std::vector<StatChange> &costs,
               const std::vector<StatChange> &yields,
               int timeCostMinutes,
               const std::vector<StatChange> &conditions,
               const std::vector<LootEntry> &loot)
    : m_name(name)
    , m_description(desc)
    , m_costs(costs)
    , m_yields(yields)
    , m_conditions(conditions)
    , m_loot(loot)
    , m_timeCost(timeCostMinutes)
{
}

QString Action::targetName(Stat t)
{
    return QString::fromUtf8(statInfo(t).displayName);
}
