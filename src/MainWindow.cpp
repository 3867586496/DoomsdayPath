#include "MainWindow.h"
#include "MainMenuPage.h"
#include "NewGamePage.h"
#include "LoadingPage.h"
#include "GamePage.h"
#include "BackpackPage.h"

#include <QApplication>
#include <QScreen>
#include <QStackedWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
}

void MainWindow::setupUI()
{
    setWindowTitle(QStringLiteral("Doomsday Path"));
    resize(800, 600);

    QScreen *screen = QApplication::primaryScreen();
    if (screen) {
        QRect geom = screen->availableGeometry();
        int x = (geom.width() - width()) / 2 + geom.x();
        int y = (geom.height() - height()) / 2 + geom.y();
        move(x, y);
    }

    setStyleSheet(QStringLiteral(
        "QMainWindow { background-color: #1a1a2e; }"
    ));

    m_stack = new QStackedWidget(this);
    setCentralWidget(m_stack);

    m_mainMenu = new MainMenuPage(this);
    m_newGame = new NewGamePage(this);
    m_loading = new LoadingPage(this);
    m_game = new GamePage(this);
    m_backpack = new BackpackPage(this);

    m_stack->addWidget(m_mainMenu);   // 0
    m_stack->addWidget(m_newGame);     // 1
    m_stack->addWidget(m_loading);     // 2
    m_stack->addWidget(m_game);        // 3
    m_stack->addWidget(m_backpack);    // 4

    // ---- 主菜单 ----
    connect(m_mainMenu, &MainMenuPage::startGameClicked, this, &MainWindow::showNewGamePage);
    connect(m_mainMenu, &MainMenuPage::loadGameClicked, this, &MainWindow::showLoadGamePage);
    connect(m_mainMenu, &MainMenuPage::exitClicked, this, &QMainWindow::close);

    // ---- 开始游戏页 ----
    connect(m_newGame, &NewGamePage::backClicked, this, &MainWindow::showMainMenu);
    connect(m_newGame, &NewGamePage::startGame, this, [this](const QString &, DifficultyFactor) {
        showGamePage();
    });

    // ---- 游戏页 ----
    connect(m_game, &GamePage::backToMenu, this, &MainWindow::showMainMenu);
    connect(m_game, &GamePage::openBackpack, this, &MainWindow::showBackpackPage);
    connect(m_backpack, &BackpackPage::closed, this, &MainWindow::showMainMenu);
    connect(m_backpack, &BackpackPage::itemUsed, m_game, &GamePage::applyItemEffects);
}

void MainWindow::showNewGamePage()  { m_stack->setCurrentWidget(m_newGame); }
void MainWindow::showLoadGamePage(){ m_stack->setCurrentWidget(m_loading); }
void MainWindow::showMainMenu()     { m_stack->setCurrentWidget(m_mainMenu); }
void MainWindow::showGamePage()
{
    m_backpack->setInventory(m_game->inventory());
    m_stack->setCurrentWidget(m_game);
}
void MainWindow::showBackpackPage() { m_stack->setCurrentWidget(m_backpack); }

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMainWindow::closeEvent(event);
}
