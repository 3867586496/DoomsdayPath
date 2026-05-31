#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "MapElement.h"
#include "Item.h"

class QStackedWidget;
class QCloseEvent;
class QKeyEvent;
class MainMenuPage;
class NewGamePage;
class LoadingPage;
class LoadGamePage;
class GamePage;
class BackpackPage;
class SettingsPage;
class GameMenuPage;
class SavePage;
class MapPage;
class SmallMapPage;
class ContainerPage;
class WorldMap;
class SaveSystem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

protected:
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void showNewGamePage();
    void showLoadGamePage();
    void showMainMenu();
    void showGamePage();
    void showBackpackPage();
    void showSettingsPage();
    void showGameMenuPage();
    void showSavePage();
    void showMapPage();
    void showSmallMapPage();
    void showContainerPage(const TileElement &container,
                           int tileX, int tileY, int buildingId);

private:
    void setupUI();
    void createPages();
    void wireMenuConnections();
    void wireGameConnections();
    void wireBackpackConnections();
    void wireSettingsConnections();
    void wireGameMenuConnections();
    void wireMapConnections();

    QStackedWidget *m_stack = nullptr;
    MainMenuPage *m_mainMenu = nullptr;
    NewGamePage *m_newGame = nullptr;
    LoadingPage *m_loading = nullptr;
    LoadGamePage *m_loadGame = nullptr;
    GamePage *m_game = nullptr;
    BackpackPage *m_backpack = nullptr;
    SettingsPage *m_settings = nullptr;
    GameMenuPage *m_gameMenu = nullptr;
    SavePage *m_savePage = nullptr;
    MapPage *m_mapPage = nullptr;
    SmallMapPage *m_smallMapPage = nullptr;
    ContainerPage *m_containerPage = nullptr;
    WorldMap *m_worldMap = nullptr;
    SaveSystem *m_saveSystem = nullptr;

    // Container item cache: key = "tileX_tileY_buildingId_containerId"
    QMap<QString, std::vector<Item>> m_containerItemCache;
    QString m_openContainerCacheKey;
    static QString containerCacheKey(int tx, int ty, int bid, int cid) {
        return QStringLiteral("%1_%2_%3_%4").arg(tx).arg(ty).arg(bid).arg(cid);
    }

    void doSave(const QString &folderName);
};

#endif // MAINWINDOW_H
