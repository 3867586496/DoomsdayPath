#ifndef MAINMENUPAGE_H
#define MAINMENUPAGE_H

#include <QWidget>

class QLabel;
class QPushButton;

class MainMenuPage : public QWidget
{
    Q_OBJECT

public:
    explicit MainMenuPage(QWidget *parent = nullptr);

signals:
    void startGameClicked();
    void loadGameClicked();
    void settingsClicked();
    void exitClicked();

private:
    void setupUI();

    QLabel *m_titleLabel = nullptr;
    QPushButton *m_btnStart = nullptr;
    QPushButton *m_btnLoad = nullptr;
    QPushButton *m_btnSettings = nullptr;
    QPushButton *m_btnExit = nullptr;
};

#endif // MAINMENUPAGE_H
