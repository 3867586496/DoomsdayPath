#ifndef GAMEMENUPAGE_H
#define GAMEMENUPAGE_H

#include <QWidget>

class QPushButton;

class GameMenuPage : public QWidget
{
    Q_OBJECT

public:
    explicit GameMenuPage(QWidget *parent = nullptr);

signals:
    void resumeClicked();
    void saveClicked();
    void exitToMainMenu();

private:
    void setupUI();
};

#endif // GAMEMENUPAGE_H
