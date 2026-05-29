#include "Action.h"

Action::Action(const QString &name, const QString &desc,
               const std::vector<StatChange> &costs,
               const std::vector<StatChange> &yields,
               int timeCostMinutes)
    : m_name(name)
    , m_description(desc)
    , m_costs(costs)
    , m_yields(yields)
    , m_timeCost(timeCostMinutes)
{
}

QString Action::targetName(StatChange::Target t)
{
    switch (t) {
    case StatChange::Hp:      return QStringLiteral("鐢熷懡");
    case StatChange::Hunger:  return QStringLiteral("楗ラタ");
    case StatChange::Thirst:  return QStringLiteral("鍙ｆ复");
    case StatChange::Sanity:  return QStringLiteral("鐞嗘櫤");
    }
    return {};
}
