#ifndef PLAYERSTATS_H
#define PLAYERSTATS_H

#include <QString>
#include <vector>
#include "StatChange.h"

// =============================================================================
// PlayerStats — array-backed attribute system driven by kStats[] registry
// =============================================================================
// Adding a stat requires adding to Stat enum + kStats[] row — see StatChange.h.
// The class auto-iterates over kStatCount for applyChanges, applyHourlyTick,
// and display strings.

class PlayerStats
{
public:
    static constexpr double DefaultMax = 100.0;

    PlayerStats();

    // Generic access
    double value(Stat s) const { return m_values[static_cast<int>(s)]; }
    void modify(Stat s, double delta);
    QString statString(Stat s) const;

    // Convenience getters (backward-compat)
    double hp() const     { return value(Stat::Hp); }
    double hunger() const { return value(Stat::Hunger); }
    double thirst() const { return value(Stat::Thirst); }
    double sanity() const { return value(Stat::Sanity); }
    double rest() const   { return value(Stat::Rest); }

    // Bulk operations (iterates the registry, no switch)
    void applyChanges(const std::vector<StatChange> &changes);
    void applyHourlyTick();

private:
    static double clamp(double val);
    double m_values[kStatCount];
};

#endif // PLAYERSTATS_H
