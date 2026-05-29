#include "GameMenuPage.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

GameMenuPage::GameMenuPage(QWidget *parent) : QWidget(parent) { setupUI(); }

void GameMenuPage::setupUI()
{
    QVBoxLayout *ml = new QVBoxLayout(this);
    ml->setContentsMargins(40, 60, 40, 60);
    ml->setAlignment(Qt::AlignCenter);

    QLabel *title = new QLabel(QStringLiteral("游戏菜单"), this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(QStringLiteral(
        "QLabel{color:#e94560;font-size:32px;font-weight:bold;padding:16px}"));
    ml->addWidget(title);
    ml->addSpacing(40);

    const QString btnStyle = QStringLiteral(
        "QPushButton{background-color:#16213e;color:#e0e0e0;"
        "border:2px solid #0f3460;border-radius:8px;"
        "font-size:20px;padding:12px 48px;min-width:240px}"
        "QPushButton:hover{background-color:#0f3460;border-color:#e94560}");

    QPushButton *btnResume = new QPushButton(QStringLiteral("继续游戏"), this);
    btnResume->setStyleSheet(btnStyle);
    btnResume->setCursor(Qt::PointingHandCursor);
    connect(btnResume, &QPushButton::clicked, this, &GameMenuPage::resumeClicked);
    ml->addWidget(btnResume, 0, Qt::AlignCenter);
    ml->addSpacing(16);

    QPushButton *btnSave = new QPushButton(QStringLiteral("保存"), this);
    btnSave->setStyleSheet(btnStyle);
    btnSave->setCursor(Qt::PointingHandCursor);
    connect(btnSave, &QPushButton::clicked, this, &GameMenuPage::saveClicked);
    ml->addWidget(btnSave, 0, Qt::AlignCenter);
    ml->addSpacing(16);

    QPushButton *btnExit = new QPushButton(QStringLiteral("退出"), this);
    btnExit->setStyleSheet(btnStyle);
    btnExit->setCursor(Qt::PointingHandCursor);
    connect(btnExit, &QPushButton::clicked, this, &GameMenuPage::exitToMainMenu);
    ml->addWidget(btnExit, 0, Qt::AlignCenter);
}
