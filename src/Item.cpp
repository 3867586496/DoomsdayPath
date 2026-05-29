#include "Item.h"

Item::Item(const QString &name, const QString &description,
           double weight, const std::vector<StatChange> &effects,
           bool edible)
    : m_name(name)
    , m_description(description)
    , m_weight(weight)
    , m_effects(effects)
    , m_edible(edible)
{
}

QString Item::functionText() const
{
    if (m_effects.empty()) {
        return QStringLiteral("-");
    }
    QStringList parts;
    for (const auto &e : m_effects) {
        QString dir = e.amount >= 0 ? QStringLiteral("+") : QString();
        parts.append(QStringLiteral("%1%2%3")
            .arg(Action::targetName(e.target))
            .arg(dir)
            .arg(e.amount, 0, 'f', 0));
    }
    return parts.join(QStringLiteral(", "));
}
