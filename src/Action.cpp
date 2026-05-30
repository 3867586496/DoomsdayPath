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

QString Action::targetName(StatChange::Target t)
{
    switch (t) {
    case StatChange::Hp:      return QStringLiteral("生命");
    case StatChange::Hunger:  return QStringLiteral("饥饿");
    case StatChange::Thirst:  return QStringLiteral("口渴");
    case StatChange::Sanity:  return QStringLiteral("理智");
    case StatChange::Rest:    return QStringLiteral("休息");
    }
    return {};
}
