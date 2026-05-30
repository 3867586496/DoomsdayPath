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
    double rest() const { return m_rest; }

    void modifyHp(double delta);
    void modifyHunger(double delta);
    void modifyThirst(double delta);
    void modifySanity(double delta);
    void modifyRest(double delta);

    void applyChanges(const std::vector<StatChange> &changes);
    void applyHourlyTick();

    QString hpString() const;
    QString hungerString() const;
    QString thirstString() const;
    QString sanityString() const;
    QString restString() const;

private:
    static double clamp(double value);

    double m_hp;
    double m_hunger;
    double m_thirst;
    double m_sanity;
    double m_rest;
};

#endif // PLAYERSTATS_H
