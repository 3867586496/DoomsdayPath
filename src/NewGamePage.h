#ifndef NEWGAMEPAGE_H
#define NEWGAMEPAGE_H

#include <QWidget>
#include "DifficultyFactor.h"

class QLineEdit;
class QPushButton;
class QRadioButton;
class QDoubleSpinBox;
class QLabel;

class NewGamePage : public QWidget
{
    Q_OBJECT

public:
    explicit NewGamePage(QWidget *parent = nullptr);

    QString saveName() const;
    DifficultyFactor difficulty() const;

signals:
    void backClicked();
    void startGame(const QString &saveName, DifficultyFactor difficulty);

private slots:
    void onPresetToggled();

private:
    void setupUI();

    QLineEdit *m_saveNameEdit = nullptr;
    QPushButton *m_btnBack = nullptr;
    QPushButton *m_btnStart = nullptr;

    // 预设难度
    QRadioButton *m_rbEasy = nullptr;
    QRadioButton *m_rbNormal = nullptr;
    QRadioButton *m_rbHard = nullptr;
    QRadioButton *m_rbHell = nullptr;
    QRadioButton *m_rbCustom = nullptr;

    // 自定义难度输入
    QWidget *m_customContainer = nullptr;
    QDoubleSpinBox *m_customFactorSpin = nullptr;
};

#endif // NEWGAMEPAGE_H
