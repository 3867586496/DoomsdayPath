// =============================================================================
// MainWindow — Application shell & page router
// =============================================================================
// Owns all pages in a QStackedWidget. Acts as the central nervous system:
// every page only knows how to emit signals; MainWindow wires those signals
// to page transitions and data synchronisation.
//
// Container item persistence: container items live in m_containerItemCache,
// keyed by "tileX_tileY_buildingId_containerId". On ContainerPage::closed
// the cache is updated; on showContainerPage it's restored. Cleared when
// starting a new game to prevent cross-game contamination.
//
// Stack indices:
//   0=mainMenu  1=newGame  2=loadGame  3=game  4=backpack
//   5=settings  6=gameMenu  7=savePage  8=mapPage  9=smallMapPage
//   10=loading  11=containerPage

#include "MainWindow.h"
#include "MainMenuPage.h"
#include "NewGamePage.h"
#include "LoadingPage.h"
#include "LoadGamePage.h"
#include "GamePage.h"
#include "BackpackPage.h"
#include "SettingsPage.h"
#include "GameMenuPage.h"
#include "SavePage.h"
#include "MapPage.h"
#include "SmallMapPage.h"
#include "ContainerPage.h"
#include "WorldMap.h"
#include "SaveSystem.h"
#include "MapElement.h"

#include <QApplication>
#include <QKeyEvent>
#include <QMessageBox>
#include <QScreen>
#include <QStackedWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
}

void MainWindow::setupUI()
{
    setWindowTitle(QStringLiteral("末日迷途"));
    resize(800, 600);

    QScreen *screen = QApplication::primaryScreen();
    if (screen) {
        QRect geom = screen->availableGeometry();
        int x = (geom.width() - width()) / 2 + geom.x();
        int y = (geom.height() - height()) / 2 + geom.y();
        move(x, y);
    }

    setStyleSheet(QStringLiteral(
        "QMainWindow { background-color: #1a1a2e; }"));

    m_stack = new QStackedWidget(this);
    setCentralWidget(m_stack);

    // ---- Create all pages ----
    m_mainMenu     = new MainMenuPage(this);
    m_newGame      = new NewGamePage(this);
    m_loading      = new LoadingPage(this);
    m_worldMap     = new WorldMap(this);
    m_game         = new GamePage(m_worldMap, this);
    m_backpack     = new BackpackPage(this);
    m_settings     = new SettingsPage(this);
    m_gameMenu     = new GameMenuPage(this);
    m_saveSystem   = new SaveSystem(this);
    m_savePage     = new SavePage(m_saveSystem, this);
    m_mapPage      = new MapPage(m_worldMap, this);
    m_smallMapPage = new SmallMapPage(m_worldMap, this);
    m_loadGame     = new LoadGamePage(m_saveSystem, this);
    m_containerPage = new ContainerPage(this);

    // Stack indices
    m_stack->addWidget(m_mainMenu);      // 0
    m_stack->addWidget(m_newGame);       // 1
    m_stack->addWidget(m_loadGame);      // 2
    m_stack->addWidget(m_game);          // 3
    m_stack->addWidget(m_backpack);      // 4
    m_stack->addWidget(m_settings);      // 5
    m_stack->addWidget(m_gameMenu);      // 6
    m_stack->addWidget(m_savePage);      // 7
    m_stack->addWidget(m_mapPage);       // 8
    m_stack->addWidget(m_smallMapPage);  // 9
    m_stack->addWidget(m_loading);       // 10
    m_stack->addWidget(m_containerPage); // 11

    // ---- 主菜单 ----
    connect(m_mainMenu, &MainMenuPage::startGameClicked,
            this, &MainWindow::showNewGamePage);
    connect(m_mainMenu, &MainMenuPage::loadGameClicked,
            this, &MainWindow::showLoadGamePage);
    connect(m_loadGame, &LoadGamePage::backClicked,
            this, &MainWindow::showMainMenu);
    connect(m_loadGame, &LoadGamePage::loadRequested, this,
            [this](const QString &folder, int entryIdx) {
                auto entries = m_saveSystem->entriesInFolder(folder);
                if (entryIdx < 0 || entryIdx >= static_cast<int>(entries.size()))
                    return;
                // TODO: actually load game state from SaveEntry
                showGamePage();
            });
    connect(m_mainMenu, &MainMenuPage::settingsClicked,
            this, &MainWindow::showSettingsPage);
    connect(m_mainMenu, &MainMenuPage::exitClicked,
            this, &QMainWindow::close);

    // ---- 开始游戏页 ----
    connect(m_newGame, &NewGamePage::backClicked,
            this, &MainWindow::showMainMenu);
    connect(m_newGame, &NewGamePage::startGame, this,
            [this](const QString &, DifficultyFactor) { showGamePage(); });

    // ---- 游戏页 ----
    connect(m_game, &GamePage::backToMenu,
            this, &MainWindow::showMainMenu);
    connect(m_game, &GamePage::openBackpack,
            this, &MainWindow::showBackpackPage);
    connect(m_game, &GamePage::openGameMenu,
            this, &MainWindow::showGameMenuPage);
    connect(m_game, &GamePage::openBigMap,
            this, &MainWindow::showMapPage);
    connect(m_game, &GamePage::openSmallMap,
            this, &MainWindow::showSmallMapPage);

    // Auto-save
    connect(m_game, &GamePage::autoSaveTriggered, this, [this]() {
        SaveEntry entry;
        entry.name = QStringLiteral("auto");
        entry.day = m_game->gameTime().day();
        entry.hour = m_game->gameTime().hour();
        entry.minute = m_game->gameTime().minute();
        entry.hp = m_game->stats().hp();
        entry.hunger = m_game->stats().hunger();
        entry.thirst = m_game->stats().thirst();
        entry.sanity = m_game->stats().sanity();
        entry.rest = m_game->stats().rest();
        entry.timestamp = QDateTime::currentDateTime();
        entry.isAuto = true;
        auto *inv = m_game->inventory();
        for (int i = 0; i < inv->count(); ++i) {
            const Item *item = inv->itemAt(i);
            if (item) entry.items.push_back(*item);
        }
        m_saveSystem->saveEntry(QStringLiteral("auto"), entry);
    });

    // Building entry / exit
    connect(m_game, &GamePage::enterBuildingRequested, this,
            [this](const TileElement &building, int tileX, int tileY) {
                m_game->enterBuilding(building.id);
            });
    connect(m_game, &GamePage::leaveBuildingRequested, this,
            [this]() {
                // Already handled in GamePage::leaveBuilding(), just refresh map
            });

    // Container open
    connect(m_game, &GamePage::openContainerRequested, this,
            [this](const TileElement &container, int tileX, int tileY, int buildingId) {
                showContainerPage(container, tileX, tileY, buildingId);
            });

    // ---- 背包页 ----
    connect(m_backpack, &BackpackPage::closed,
            this, &MainWindow::showGamePage);
    connect(m_backpack, &BackpackPage::itemUsed,
            m_game, &GamePage::applyItemEffects);

    // ---- 设置页 ----
    connect(m_settings, &SettingsPage::backClicked,
            this, &MainWindow::showMainMenu);
    connect(m_settings, &SettingsPage::resolutionChanged,
            this, [this](int w, int h) { resize(w, h); });
    connect(m_settings, &SettingsPage::fullscreenToggled,
            this, [this](bool fs) {
                if (fs) showFullScreen(); else showNormal();
                showMainMenu();
            });
    connect(m_settings, &SettingsPage::autoSaveSettingsChanged,
            this, [this](int interval, int count) {
                m_game->setAutoSaveInterval(interval);
                m_saveSystem->setMaxAutoSaves(count);
            });

    // ---- 游戏菜单 ----
    connect(m_gameMenu, &GameMenuPage::resumeClicked,
            this, &MainWindow::showGamePage);
    connect(m_gameMenu, &GameMenuPage::saveClicked,
            this, &MainWindow::showSavePage);
    connect(m_gameMenu, &GameMenuPage::exitToMainMenu,
            this, &MainWindow::showMainMenu);

    // ---- 存档页 ----
    connect(m_savePage, &SavePage::cancelled,
            this, &MainWindow::showGameMenuPage);
    connect(m_savePage, &SavePage::saveRequested, this,
            [this](const QString &folderName) { doSave(folderName); });

    // ---- 地图页 ----
    connect(m_mapPage, &MapPage::returnToGame,
            this, &MainWindow::showGamePage);
    connect(m_mapPage, &MapPage::tileEntered, this,
            [this](int, int, int cost) {
                if (m_game->isIndoor()) {
                    QMessageBox::information(this, QStringLiteral("无法移动"),
                        QStringLiteral("在建筑内无法在大地图上移动"));
                    return;
                }
                m_game->advanceTime(cost);
                m_game->refreshTileContext();
            });

    // ---- 小地图页 ----
    connect(m_smallMapPage, &SmallMapPage::returnToGame,
            this, &MainWindow::showGamePage);

    // ---- 容器页 ----
    connect(m_containerPage, &ContainerPage::closed, this, [this]() {
        // Save container items to cache before leaving
        if (!m_openContainerCacheKey.isEmpty())
            m_containerItemCache[m_openContainerCacheKey] = m_containerPage->containerItems();
        m_openContainerCacheKey.clear();
        showGamePage();
    });
    connect(m_containerPage, &ContainerPage::containerSearched, this,
            [this](int tileX, int tileY, int buildingId, int containerId) {
                m_worldMap->markContainerOpened(tileX, tileY, buildingId, containerId);
                m_game->containerOpened(buildingId, containerId);
            });
    connect(m_containerPage, &ContainerPage::itemTransferredToBackpack,
            this, [this](const Item &item, int count) {
                for (int i = 0; i < count; ++i)
                    m_game->inventory()->addItem(item);
            });
    connect(m_containerPage, &ContainerPage::itemTransferredToContainer,
            this, [this](const Item &item, int count) {
                auto *inv = m_game->inventory();
                for (int c = 0; c < count; ++c) {
                    for (int i = 0; i < inv->count(); ++i) {
                        const Item *it = inv->itemAt(i);
                        if (it && it->name() == item.name()) {
                            inv->removeItem(i);
                            break;
                        }
                    }
                }
            });
}

// ---------------------------------------------------------------------------
// Page navigation
// ---------------------------------------------------------------------------
void MainWindow::showNewGamePage()  {
    m_containerItemCache.clear();
    m_openContainerCacheKey.clear();
    m_stack->setCurrentWidget(m_newGame);
}
void MainWindow::showLoadGamePage()
{
    m_loadGame->refreshList();
    m_stack->setCurrentWidget(m_loadGame);
}
void MainWindow::showMainMenu()     { m_stack->setCurrentWidget(m_mainMenu); }
void MainWindow::showGamePage()
{
    m_game->refreshTileContext();
    m_backpack->setInventory(m_game->inventory());
    m_backpack->setCanUseCallback([this](const std::vector<StatChange> &effects) -> bool {
        for (const auto &e : effects) {
            if (e.amount >= 0) continue;
            double current = 0;
            switch (e.target) {
            case StatChange::Hp:      current = m_game->stats().hp(); break;
            case StatChange::Hunger:  current = m_game->stats().hunger(); break;
            case StatChange::Thirst:  current = m_game->stats().thirst(); break;
            case StatChange::Sanity:  current = m_game->stats().sanity(); break;
            case StatChange::Rest:    current = m_game->stats().rest(); break;
            }
            if (current + e.amount < 0) return false;
        }
        return true;
    });
    m_stack->setCurrentWidget(m_game);
}
void MainWindow::showBackpackPage() { m_stack->setCurrentWidget(m_backpack); }
void MainWindow::showSettingsPage() { m_stack->setCurrentWidget(m_settings); }
void MainWindow::showGameMenuPage() { m_stack->setCurrentWidget(m_gameMenu); }
void MainWindow::showSavePage()
{
    m_savePage->refreshList();
    m_stack->setCurrentWidget(m_savePage);
}
void MainWindow::showMapPage()
{
    m_stack->setCurrentWidget(m_mapPage);
}
void MainWindow::showSmallMapPage()
{
    m_smallMapPage->refresh();
    m_stack->setCurrentWidget(m_smallMapPage);
}
void MainWindow::showContainerPage(const TileElement &container,
                                    int tileX, int tileY, int buildingId)
{
    m_containerPage->setContainer(container, tileX, tileY, buildingId);
    m_openContainerCacheKey = containerCacheKey(tileX, tileY, buildingId, container.id);

    // Restore cached container items if any
    QString key = containerCacheKey(tileX, tileY, buildingId, container.id);
    auto ci = m_containerItemCache.find(key);
    if (ci != m_containerItemCache.end() && !ci->empty())
        m_containerPage->setContainerItems(ci.value());

    // Pass backpack items to container page
    std::vector<Item> bpItems;
    for (int i = 0; i < m_game->inventory()->count(); ++i) {
        const Item *it = m_game->inventory()->itemAt(i);
        if (it) bpItems.push_back(*it);
    }
    m_containerPage->setBackpackInventory(bpItems);

    m_stack->setCurrentWidget(m_containerPage);
}

// ---------------------------------------------------------------------------
// Window events
// ---------------------------------------------------------------------------
void MainWindow::closeEvent(QCloseEvent *event)
{
    QMainWindow::closeEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape && isFullScreen()) {
        showNormal();
        return;
    }
    QMainWindow::keyPressEvent(event);
}

// ---------------------------------------------------------------------------
// Save
// ---------------------------------------------------------------------------
void MainWindow::doSave(const QString &folderName)
{
    SaveEntry entry;
    entry.name = folderName;
    entry.day = m_game->gameTime().day();
    entry.hour = m_game->gameTime().hour();
    entry.minute = m_game->gameTime().minute();
    entry.hp = m_game->stats().hp();
    entry.hunger = m_game->stats().hunger();
    entry.thirst = m_game->stats().thirst();
    entry.sanity = m_game->stats().sanity();
    entry.rest = m_game->stats().rest();
    entry.timestamp = QDateTime::currentDateTime();
    entry.isAuto = false;

    auto *inv = m_game->inventory();
    for (int i = 0; i < inv->count(); ++i) {
        const Item *item = inv->itemAt(i);
        if (item) entry.items.push_back(*item);
    }

    if (m_saveSystem->saveEntry(folderName, entry)) {
        QMessageBox::information(this, QStringLiteral("保存成功"),
            QStringLiteral("存档已保存"));
        m_savePage->refreshList();
    } else {
        QMessageBox::warning(this, QStringLiteral("保存失败"),
            QStringLiteral("无法保存存档"));
    }
}
