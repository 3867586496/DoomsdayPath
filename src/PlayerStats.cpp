#include "PlayerStats.h"
#include "StatChange.h"

#include <algorithm>

PlayerStats::PlayerStats()
    : m_hp(DefaultMax)
    , m_hunger(DefaultMax)
    , m_thirst(DefaultMax)
    , m_sanity(DefaultMax)
{
}

double PlayerStats::clamp(double value)
{
    return std::clamp(value, 0.0, DefaultMax);
}

void PlayerStats::modifyHp(double delta)       { m_hp = clamp(m_hp + delta); }
void PlayerStats::modifyHunger(double delta)   { m_hunger = clamp(m_hunger + delta); }
void PlayerStats::modifyThirst(double delta)   { m_thirst = clamp(m_thirst + delta); }
void PlayerStats::modifySanity(double delta)   { m_sanity = clamp(m_sanity + delta); }

void PlayerStats::applyChanges(const std::vector<StatChange> &changes)
{
    for (const auto &c : changes) {
        switch (c.target) {
        case StatChange::Hp:      modifyHp(c.amount); break;
        case StatChange::Hunger:  modifyHunger(c.amount); break;
        case StatChange::Thirst:  modifyThirst(c.amount); break;
        case StatChange::Sanity:  modifySanity(c.amount); break;
        }
    }
}

QString PlayerStats::hpString() const
{
    return QStringLiteral("生命: %1/%2").arg(m_hp, 0, 'f', 0).arg(DefaultMax, 0, 'f', 0);
}

QString PlayerStats::hungerString() const
{
    return QStringLiteral("饥饿: %1/%2").arg(m_hunger, 0, 'f', 0).arg(DefaultMax, 0, 'f', 0);
}

QString PlayerStats::thirstString() const
{
    return QStringLiteral("口渴: %1/%2").arg(m_thirst, 0, 'f', 0).arg(DefaultMax, 0, 'f', 0);
}

QString PlayerStats::sanityString() const
{
    return QStringLiteral("理智: %1/%2").arg(m_sanity, 0, 'f', 0).arg(DefaultMax, 0, 'f', 0);
}
