#ifndef GAMETIME_H
#define GAMETIME_H

#include <QString>

class GameTime
{
public:
    GameTime(int day = 1, int hour = 8, int minute = 0);

    int day() const { return m_day; }
    int hour() const { return m_hour; }
    int minute() const { return m_minute; }

    void advance(int minutes);
    QString displayString() const;

private:
    int m_day;
    int m_hour;
    int m_minute;

    void normalize();
};

#endif // GAMETIME_H
