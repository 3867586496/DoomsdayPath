#ifndef PLAYERSTATS_H
#define PLAYERSTATS_H

#include <QString>
#include <vector>
#include "StatChange.h"

class PlayerStats
{
public:
    static constexpr double DefaultMax = 100.0;

    PlayerStats();

    double hp() const { return m_hp; }
    double hunger() const { return m_hunger; }
    double thirst() const { return m_thirst; }
    double sanity() const { return m_sanity; }

    void modifyHp(double delta);
    void modifyHunger(double delta);
    void modifyThirst(double delta);
    void modifySanity(double delta);

    void applyChanges(const std::vector<StatChange> &changes);

    QString hpString() const;
    QString hungerString() const;
    QString thirstString() const;
    QString sanityString() const;

private:
    static double clamp(double value);

    double m_hp;
    double m_hunger;
    double m_thirst;
    double m_sanity;
};

#endif // PLAYERSTATS_H
