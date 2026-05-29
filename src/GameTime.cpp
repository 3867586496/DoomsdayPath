#include "GameTime.h"

GameTime::GameTime(int day, int hour, int minute)
    : m_day(day), m_hour(hour), m_minute(minute)
{
    normalize();
}

void GameTime::advance(int minutes)
{
    m_minute += minutes;
    normalize();
}

void GameTime::normalize()
{
    while (m_minute >= 60) {
        m_minute -= 60;
        m_hour += 1;
    }
    while (m_minute < 0) {
        m_minute += 60;
        m_hour -= 1;
    }
    while (m_hour >= 24) {
        m_hour -= 24;
        m_day += 1;
    }
    while (m_hour < 0) {
        m_hour += 24;
        m_day -= 1;
    }
    if (m_day < 1) m_day = 1;
}

QString GameTime::displayString() const
{
    return QStringLiteral("第%1天 %2:%3")
        .arg(m_day)
        .arg(m_hour, 2, 10, QLatin1Char('0'))
        .arg(m_minute, 2, 10, QLatin1Char('0'));
}
