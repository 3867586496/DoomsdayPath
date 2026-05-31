#include "PlayerStats.h"
#include "StatChange.h"

#include <algorithm>

// =============================================================================
// PlayerStats — Array-backed stat system
// =============================================================================
// m_values[] is indexed by Stat enum value. Initialised to DefaultMax (100).
// modify() clamps to [0, max]. applyHourlyTick implements the hunger/thirst/
// rest decay + conditional healing; it uses Stat enum values directly instead
// of named members, so adding a new stat auto-includes it in the decay loop.

PlayerStats::PlayerStats()
{
    for (int i = 0; i < kStatCount; ++i)
        m_values[i] = DefaultMax;
}

double PlayerStats::clamp(double val)
{
    return std::clamp(val, 0.0, DefaultMax);
}

void PlayerStats::modify(Stat s, double delta)
{
    int idx = static_cast<int>(s);
    m_values[idx] = clamp(m_values[idx] + delta);
}

void PlayerStats::applyChanges(const std::vector<StatChange> &changes)
{
    for (const auto &c : changes)
        modify(c.target, c.amount);
}

void PlayerStats::applyHourlyTick()
{
    // Pre-tick healing eligibility
    bool canHeal = (value(Stat::Hp) < DefaultMax
                    && value(Stat::Hunger) > 50
                    && value(Stat::Thirst)  > 50
                    && value(Stat::Rest)    > 50);

    // Decay: hunger, thirst, rest lose 2 per hour
    double preHunger = value(Stat::Hunger);
    double preThirst = value(Stat::Thirst);
    double preRest   = value(Stat::Rest);
    modify(Stat::Hunger, -2);
    modify(Stat::Thirst, -2);
    modify(Stat::Rest,   -2);

    // Shortfall from clamping → HP damage
    double shortfall = 6.0 - ((preHunger - value(Stat::Hunger))
                             + (preThirst - value(Stat::Thirst))
                             + (preRest   - value(Stat::Rest)));
    if (shortfall > 0)
        modify(Stat::Hp, -shortfall);

    // Healing: sacrifice 3 from each survival stat for 3 HP
    if (canHeal) {
        modify(Stat::Hunger, -3);
        modify(Stat::Thirst, -3);
        modify(Stat::Rest,   -3);
        modify(Stat::Hp,     +3);
    }
}

QString PlayerStats::statString(Stat s) const
{
    auto &info = statInfo(s);
    return QStringLiteral("%1: %2/%3")
        .arg(QString::fromUtf8(info.displayName))
        .arg(m_values[static_cast<int>(s)], 0, 'f', 0)
        .arg(info.max, 0, 'f', 0);
}
