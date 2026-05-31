#ifndef CONTAINERPAGE_H
#define CONTAINERPAGE_H

#include <QWidget>
#include <vector>
#include "Item.h"
#include "MapElement.h"

class QLabel;
class QTableWidget;
class QPushButton;

class ContainerPage : public QWidget
{
    Q_OBJECT

public:
    explicit ContainerPage(QWidget *parent = nullptr);

    void setContainer(const TileElement &container,
                      int tileX, int tileY, int buildingId);
    void setBackpackInventory(const std::vector<Item> &items);

signals:
    void closed();
    void containerSearched(int tileX, int tileY, int buildingId, int containerId);
    void itemTransferredToBackpack(const Item &item, int count);
    void itemTransferredToContainer(const Item &item, int count);

private slots:
    void refresh();
    void onTransferToBackpack(int row);
    void onTransferToContainer(int row);

private:
    void setupUI();
    void populateContainerItems();
    void populateBackpackItems();

    TileElement m_container;
    int m_containerTileX = 0;
    int m_containerTileY = 0;
    int m_buildingId = 0;

    std::vector<Item> m_containerItems;
    std::vector<Item> m_backpackItems;

    QLabel *m_titleLabel = nullptr;
    QTableWidget *m_containerTable = nullptr;
    QTableWidget *m_backpackTable = nullptr;
    QPushButton *m_btnReturn = nullptr;
    QPushButton *m_btnSearch = nullptr;
};

#endif // CONTAINERPAGE_H
