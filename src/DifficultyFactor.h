#ifndef DIFFICULTYFACTOR_H
#define DIFFICULTYFACTOR_H

#include <QString>

struct DifficultyPreset
{
    QString name;
    double factor;
};

class DifficultyFactor
{
public:
    static constexpr double Default = 1.0;
    static constexpr double Min = 0.0;
    static constexpr double Max = 100.0;

    static const DifficultyPreset &presetEasy();
    static const DifficultyPreset &presetNormal();
    static const DifficultyPreset &presetHard();
    static const DifficultyPreset &presetHell();

    static const DifficultyPreset *allPresets();
    static int presetCount();

    DifficultyFactor() : m_factor(Default) {}
    explicit DifficultyFactor(double factor) : m_factor(clamp(factor)) {}

    double factor() const { return m_factor; }
    void setFactor(double f) { m_factor = clamp(f); }

    QString displayString() const;

private:
    static double clamp(double v);
    double m_factor;
};

#endif // DIFFICULTYFACTOR_H
