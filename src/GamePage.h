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
    const PlayerStats &stats() const { return m_stats; }
    const GameTime &gameTime() const { return m_time; }
    void advanceTime(int minutes) {
        processHourlyTicks(minutes);
        m_time.advance(minutes);
        refreshStats();
    }

    int autoSaveInterval() const { return m_autoSaveInterval; }
    void setAutoSaveInterval(int days) { m_autoSaveInterval = days; }

public slots:
    void applyItemEffects(const std::vector<StatChange> &effects);

signals:
    void backToMenu();
    void openBackpack();
    void openGameMenu();
    void openBigMap();
    void openSmallMap();
    void autoSaveTriggered();

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
    void processHourlyTicks(int minutesPassed);

    PlayerStats m_stats;
    GameTime m_time;
    Inventory *m_inventory = nullptr;

    QLabel *m_hpLabel = nullptr;
    QLabel *m_hungerLabel = nullptr;
    QLabel *m_thirstLabel = nullptr;
    QLabel *m_sanityLabel = nullptr;
    QLabel *m_restLabel = nullptr;
    QLabel *m_timeLabel = nullptr;

    QVBoxLayout *m_actionLayout = nullptr;
    QWidget *m_testArea = nullptr;
    QLabel *m_invFullLabel = nullptr;
    int m_autoSaveInterval = 1;  // days, default 1
};

#endif // GAMEPAGE_H
