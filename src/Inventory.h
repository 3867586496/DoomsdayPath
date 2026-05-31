#ifndef INVENTORY_H
#define INVENTORY_H

#include <QObject>
#include <vector>
#include "Item.h"

class Inventory : public QObject
{
    Q_OBJECT

public:
    static constexpr double DefaultCapacity = 30.0;

    explicit Inventory(QObject *parent = nullptr);

    double capacity() const { return m_capacity; }
    double currentWeight() const;
    const std::vector<Item> &items() const { return m_items; }
    int count() const { return m_items.size(); }

    bool canAdd(const Item &item) const;
    bool addItem(const Item &item);
    void removeItem(int index);
    void clear();
    const Item *itemAt(int index) const;

signals:
    void changed();

private:
    std::vector<Item> m_items;
    double m_capacity;
};

#endif // INVENTORY_H
