#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

private:
    void setupUI();

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
    SaveSystem *m_saveSystem = nullptr;

    void doSave(const QString &folderName);
};

#endif // MAINWINDOW_H
