#ifndef GAMEPAGE_H
#define GAMEPAGE_H

#include <QWidget>
#include "PlayerStats.h"
#include "GameTime.h"
#include "Action.h"
#include "Inventory.h"
#include "Item.h"

class QLabel;
class QPushButton;
class QVBoxLayout;

class GamePage : public QWidget
{
    Q_OBJECT

public:
    explicit GamePage(QWidget *parent = nullptr);
    Inventory *inventory() { return m_inventory; }
public slots:
    void applyItemEffects(const std::vector<StatChange> &effects);

signals:
    void backToMenu();
    void openBackpack();

private slots:
    void onActionClicked(const Action &action);
    void onBackpackClicked();

private:
    void setupUI();
    void setupTestActions();
    void setupTestItems();
    void refreshStats();
    QPushButton *createActionButton(const Action &action);

    double statValue(StatChange::Target t) const;
    bool checkConditions(const Action &action);

    PlayerStats m_stats;
    GameTime m_time;
    Inventory *m_inventory = nullptr;

    QLabel *m_hpLabel = nullptr;
    QLabel *m_hungerLabel = nullptr;
    QLabel *m_thirstLabel = nullptr;
    QLabel *m_sanityLabel = nullptr;
    QLabel *m_timeLabel = nullptr;

    QVBoxLayout *m_actionLayout = nullptr;
    QWidget *m_testArea = nullptr;
};

#endif // GAMEPAGE_H
