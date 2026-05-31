#ifndef STATCHANGE_H
#define STATCHANGE_H

// =============================================================================
// Stat registry — add a row to introduce a new character attribute
// =============================================================================
// Adding a stat requires:
//   1. One enum value (before Count)
//   2. One row in kStats[]
//   3. One QLabel + creation in GamePage::setupUI / refreshStats
//   4. One case in MainWindow auto-save / canUseCallback
// Everything else (applyChanges, applyHourlyTick, display strings)
// is driven by the registry automatically.

enum class Stat : int {
    Hp, Hunger, Thirst, Sanity, Rest,
    Count  // sentinel — always last
};

struct StatInfo {
    Stat id;
    const char *name;         // internal key: "hp"
    const char *displayName;  // Chinese label: "生命"
    double max;               // ceiling value (all stats share 100 for now)
};

constexpr StatInfo kStats[] = {
    { Stat::Hp,     "hp",     "生命", 100 },
    { Stat::Hunger, "hunger", "饥饿", 100 },
    { Stat::Thirst, "thirst", "口渴", 100 },
    { Stat::Sanity, "sanity", "理智", 100 },
    { Stat::Rest,   "rest",   "休息", 100 },
};
constexpr int kStatCount = sizeof(kStats) / sizeof(kStats[0]);

inline const StatInfo &statInfo(Stat s) {
    for (int i = 0; i < kStatCount; ++i)
        if (kStats[i].id == s) return kStats[i];
    return kStats[0];
}

struct StatChange {
    Stat target;
    double amount; // 正值=获得，负值=消耗
};

#endif // STATCHANGE_H
