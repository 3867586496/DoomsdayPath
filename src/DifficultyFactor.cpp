#include "DifficultyFactor.h"

#include <cmath>

const DifficultyPreset &DifficultyFactor::presetEasy()
{
    static const DifficultyPreset p{QStringLiteral("简单"), 0.5};
    return p;
}

const DifficultyPreset &DifficultyFactor::presetNormal()
{
    static const DifficultyPreset p{QStringLiteral("正常"), 1.0};
    return p;
}

const DifficultyPreset &DifficultyFactor::presetHard()
{
    static const DifficultyPreset p{QStringLiteral("困难"), 2.0};
    return p;
}

const DifficultyPreset &DifficultyFactor::presetHell()
{
    static const DifficultyPreset p{QStringLiteral("地狱"), 5.0};
    return p;
}

const DifficultyPreset *DifficultyFactor::allPresets()
{
    static const DifficultyPreset presets[] = {
        presetEasy(), presetNormal(), presetHard(), presetHell()
    };
    return presets;
}

int DifficultyFactor::presetCount()
{
    return 4;
}

double DifficultyFactor::clamp(double v)
{
    if (v < Min) return Min;
    if (v > Max) return Max;
    return v;
}

QString DifficultyFactor::displayString() const
{
    return QStringLiteral("x%1").arg(m_factor, 0, 'f', 1);
}
