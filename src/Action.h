#ifndef ACTION_H
#define ACTION_H

#include <QString>
#include <vector>

struct StatChange
{
    enum Target { Hp, Hunger, Thirst, Sanity };
    Target target;
    double amount; // 姝ｅ€?鑾峰緱锛岃礋鍊?娑堣€?
};

class Action
{
public:
    Action() = default;
    Action(const QString &name, const QString &desc,
           const std::vector<StatChange> &costs = {},
           const std::vector<StatChange> &yields = {},
           int timeCostMinutes = 0);

    QString name() const { return m_name; }
    QString description() const { return m_description; }
    const std::vector<StatChange> &costs() const { return m_costs; }
    const std::vector<StatChange> &yields() const { return m_yields; }
    int timeCostMinutes() const { return m_timeCost; }

    static QString targetName(StatChange::Target t);

private:
    QString m_name;
    QString m_description;
    std::vector<StatChange> m_costs;
    std::vector<StatChange> m_yields;
    int m_timeCost = 0;
};

#endif // ACTION_H
