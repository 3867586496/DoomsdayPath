#ifndef BACKPACKPAGE_H
#define BACKPACKPAGE_H

#include <QWidget>
#include "Inventory.h"

class QLabel;
class QTableWidget;
class QPushButton;

class BackpackPage : public QWidget
{
    Q_OBJECT

public:
    explicit BackpackPage(QWidget *parent = nullptr);

    void setInventory(Inventory *inventory);

signals:
    void closed();
    void itemUsed(const std::vector<StatChange> &effects);

private slots:
    void refresh();
    void onDiscardItem(int row);
    void onUseItem(int row);

private:
    void setupUI();

    Inventory *m_inventory = nullptr;

    QLabel *m_weightLabel = nullptr;
    QTableWidget *m_table = nullptr;
    QPushButton *m_btnClose = nullptr;
};

#endif // BACKPACKPAGE_H
