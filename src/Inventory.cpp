#include "Inventory.h"

Inventory::Inventory(QObject *parent)
    : QObject(parent)
    , m_capacity(DefaultCapacity)
{
}

double Inventory::currentWeight() const
{
    double total = 0.0;
    for (const auto &item : m_items) {
        total += item.weight();
    }
    return total;
}

bool Inventory::canAdd(const Item &item) const
{
    return currentWeight() + item.weight() <= m_capacity;
}

bool Inventory::addItem(const Item &item)
{
    if (!canAdd(item)) return false;
    m_items.push_back(item);
    emit changed();
    return true;
}

void Inventory::removeItem(int index)
{
    if (index < 0 || index >= static_cast<int>(m_items.size())) return;
    m_items.erase(m_items.begin() + index);
    emit changed();
}

const Item *Inventory::itemAt(int index) const
{
    if (index < 0 || index >= static_cast<int>(m_items.size())) return nullptr;
    return &m_items.at(index);
}
