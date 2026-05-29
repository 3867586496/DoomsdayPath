#include "MainWindow.h"
#include "MainMenuPage.h"
#include "NewGamePage.h"
#include "LoadingPage.h"
#include "LoadGamePage.h"
#include "GamePage.h"
#include "BackpackPage.h"
#include "SettingsPage.h"
#include "GameMenuPage.h"
#include "SaveSystem.h"

#include <QInputDialog>
#include <QMessageBox>

#include <QApplication>
#include <QKeyEvent>
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

    m_mainMenu = new MainMenuPage(this);
    m_newGame = new NewGamePage(this);
    m_loading = new LoadingPage(this);
    m_loadGame = new LoadGamePage(m_saveSystem, this);
    m_game = new GamePage(this);
    m_backpack = new BackpackPage(this);
    m_settings = new SettingsPage(this);
    m_gameMenu = new GameMenuPage(this);
    m_saveSystem = new SaveSystem(this);

    m_stack->addWidget(m_mainMenu);   // 0
    m_stack->addWidget(m_newGame);     // 1
    m_stack->addWidget(m_loadGame);    // 2
    m_stack->addWidget(m_game);        // 3
    m_stack->addWidget(m_backpack);    // 4
    m_stack->addWidget(m_settings);    // 5
    m_stack->addWidget(m_gameMenu);    // 6
    m_stack->addWidget(m_loading);     // 7

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
                auto &e = entries[entryIdx];
                // TODO: actually load game state from SaveEntry
                // For now, just go to game page
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

    // ---- 游戏菜单 ----
    connect(m_gameMenu, &GameMenuPage::resumeClicked,
            this, &MainWindow::showGamePage);
    connect(m_gameMenu, &GameMenuPage::saveClicked,
            this, &MainWindow::doSave);
    connect(m_gameMenu, &GameMenuPage::exitToMainMenu,
            this, &MainWindow::showMainMenu);
}

void MainWindow::showNewGamePage()  { m_stack->setCurrentWidget(m_newGame); }
void MainWindow::showLoadGamePage()
{
    m_loadGame->refreshList();
    m_stack->setCurrentWidget(m_loadGame);
}
void MainWindow::showMainMenu()     { m_stack->setCurrentWidget(m_mainMenu); }
void MainWindow::showGamePage()
{
    m_backpack->setInventory(m_game->inventory());
    m_stack->setCurrentWidget(m_game);
}
void MainWindow::showBackpackPage() { m_stack->setCurrentWidget(m_backpack); }
void MainWindow::showSettingsPage() { m_stack->setCurrentWidget(m_settings); }
void MainWindow::showGameMenuPage() { m_stack->setCurrentWidget(m_gameMenu); }

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

void MainWindow::doSave()
{
    // Prompt for save folder name
    bool ok;
    QString folderName = QInputDialog::getText(
        this, QStringLiteral("保存"),
        QStringLiteral("存档名称:"),
        QLineEdit::Normal,
        QStringLiteral("存档1"), &ok);
    if (!ok || folderName.isEmpty()) return;

    // Build save entry from current game state
    SaveEntry entry;
    entry.name = folderName;
    entry.day = m_game->gameTime().day();
    entry.hour = m_game->gameTime().hour();
    entry.minute = m_game->gameTime().minute();
    entry.hp = m_game->stats().hp();
    entry.hunger = m_game->stats().hunger();
    entry.thirst = m_game->stats().thirst();
    entry.sanity = m_game->stats().sanity();
    entry.timestamp = QDateTime::currentDateTime();
    entry.isAuto = false;

    // Copy inventory items
    auto *inv = m_game->inventory();
    for (int i = 0; i < inv->count(); ++i) {
        const Item *item = inv->itemAt(i);
        if (item) entry.items.push_back(*item);
    }

    if (m_saveSystem->saveEntry(folderName, entry)) {
        QMessageBox::information(this, QStringLiteral("保存成功"),
            QStringLiteral("存档已保存"));
        showMainMenu();
    } else {
        QMessageBox::warning(this, QStringLiteral("保存失败"),
            QStringLiteral("无法保存存档"));
    }
}
