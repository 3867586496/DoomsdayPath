#include "PlayerStats.h"
#include "StatChange.h"

#include <algorithm>

PlayerStats::PlayerStats()
    : m_hp(DefaultMax)
    , m_hunger(DefaultMax)
    , m_thirst(DefaultMax)
    , m_sanity(DefaultMax)
    , m_rest(DefaultMax)
{
}

double PlayerStats::clamp(double value)
{
    return std::clamp(value, 0.0, DefaultMax);
}

void PlayerStats::modifyHp(double delta)     { m_hp = clamp(m_hp + delta); }
void PlayerStats::modifyHunger(double delta) { m_hunger = clamp(m_hunger + delta); }
void PlayerStats::modifyThirst(double delta) { m_thirst = clamp(m_thirst + delta); }
void PlayerStats::modifySanity(double delta) { m_sanity = clamp(m_sanity + delta); }
void PlayerStats::modifyRest(double delta)   { m_rest = clamp(m_rest + delta); }

void PlayerStats::applyChanges(const std::vector<StatChange> &changes)
{
    for (const auto &c : changes) {
        switch (c.target) {
        case StatChange::Hp:      modifyHp(c.amount); break;
        case StatChange::Hunger:  modifyHunger(c.amount); break;
        case StatChange::Thirst:  modifyThirst(c.amount); break;
        case StatChange::Sanity:  modifySanity(c.amount); break;
        case StatChange::Rest:    modifyRest(c.amount); break;
        }
    }
}

void PlayerStats::applyHourlyTick()
{
    // Step 1: check pre-tick values for healing decision
    bool canHeal = (m_hp < DefaultMax && m_hunger > 50 && m_thirst > 50 && m_rest > 50);

    // Step 2: deduct hunger/thirst/rest by 2 each hour
    double preHunger = m_hunger, preThirst = m_thirst, preRest = m_rest;
    modifyHunger(-2); modifyThirst(-2); modifyRest(-2);

    // Shortfall from clamped values goes to HP
    double actualHunger = preHunger - m_hunger;
    double actualThirst = preThirst - m_thirst;
    double actualRest   = preRest - m_rest;
    double expected = 6.0;
    double shortfall = expected - (actualHunger + actualThirst + actualRest);
    if (shortfall > 0) modifyHp(-shortfall);

    // Step 3: if HP is not full and all three were > 50% before deduction, sacrifice to heal
    if (canHeal) {
        modifyHunger(-3); modifyThirst(-3); modifyRest(-3);
        modifyHp(3);
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

QString PlayerStats::restString() const
{
    return QStringLiteral("休息: %1/%2").arg(m_rest, 0, 'f', 0).arg(DefaultMax, 0, 'f', 0);
}
