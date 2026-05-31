#ifndef GAMEPAGE_H
#define GAMEPAGE_H

#include <QWidget>
#include "PlayerStats.h"
#include "GameTime.h"
#include "Inventory.h"
#include "Item.h"
#include "MapTile.h"
#include "MapElement.h"

class QLabel;
class QPushButton;
class QTableWidget;
class WorldMap;

// =============================================================================
// GamePage — Primary gameplay screen
// =============================================================================
// Layout: top-left stats | top-right location info | centre element table |
//         bottom nav buttons (big map, small map, backpack, game menu).
//
// Element table contents depend on whether the player is outdoors (tile
// elements: trees, stones, houses, trash cans) or inside a building
// (interior elements: typically just a trash can).
//
// Time flows through actions (gather, enter, search)—each action advances
// the clock by a fixed number of minutes and triggers hourly ticks.
class GamePage : public QWidget
{
    Q_OBJECT

public:
    explicit GamePage(WorldMap *map, QWidget *parent = nullptr);
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

    // Full state reset — called when starting a new game
    void resetGame();

    // Called by MainWindow when map movement happens — update tile context
    void refreshTileContext();

    // Building navigation
    void enterBuilding(int buildingId);
    void leaveBuilding();
    bool isIndoor() const { return m_inBuilding; }

    // Container interaction — called when returning from ContainerPage
    void containerOpened(int buildingId, int containerId);

public slots:
    void applyItemEffects(const std::vector<StatChange> &effects);

signals:
    void backToMenu();
    void openBackpack();
    void openGameMenu();
    void openBigMap();
    void openSmallMap();
    void autoSaveTriggered();
    void enterBuildingRequested(const TileElement &building, int tileX, int tileY);
    void leaveBuildingRequested();
    void openContainerRequested(const TileElement &container,
                                int tileX, int tileY, int buildingId);

private slots:
    void onElementAction(int row);
    void refreshElementTable();

private:
    void setupUI();
    void setupTestItems();
    void refreshStats();
    void refreshLocationInfo();
    void applyGather(const TileElement &elem);
    void processHourlyTicks(int minutesPassed);

    double statValue(Stat t) const;
    bool checkStatCosts(const std::vector<StatChange> &costs);

    WorldMap *m_map;
    PlayerStats m_stats;
    GameTime m_time;
    Inventory *m_inventory = nullptr;

    // Stats labels (top-left)
    QLabel *m_hpLabel = nullptr;
    QLabel *m_hungerLabel = nullptr;
    QLabel *m_thirstLabel = nullptr;
    QLabel *m_sanityLabel = nullptr;
    QLabel *m_restLabel = nullptr;

    // Location info (top-right)
    QLabel *m_timeLabel = nullptr;
    QLabel *m_tileLabel = nullptr;
    QLabel *m_locLabel = nullptr;

    // Element table (center)
    QTableWidget *m_elemTable = nullptr;

    // Bottom buttons
    QPushButton *m_btnBigMap = nullptr;
    QPushButton *m_btnSmallMap = nullptr;
    QPushButton *m_btnBackpack = nullptr;
    QPushButton *m_btnGameMenu = nullptr;

    // Inventory full warning
    QLabel *m_invFullLabel = nullptr;

    // Building state
    bool m_inBuilding = false;
    int m_buildingId = 0;
    int m_buildingTileX = 0;
    int m_buildingTileY = 0;
    std::vector<TileElement> m_currentElements; // either tile elements or building interior
    QString m_buildingName;

    int m_autoSaveInterval = 1;
};

#endif // GAMEPAGE_H
