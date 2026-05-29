#include "MainMenuPage.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

MainMenuPage::MainMenuPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void MainMenuPage::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    layout->addStretch(3);

    // 标题
    m_titleLabel = new QLabel(QStringLiteral("末日迷途"), this);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet(QStringLiteral(
        "QLabel {"
        "  color: #e94560;"
        "  font-size: 48px;"
        "  font-weight: bold;"
        "  padding: 20px;"
        "}"
    ));
    layout->addWidget(m_titleLabel);

    layout->addSpacing(40);

    // 按钮样式模板
    const QString buttonStyle = QStringLiteral(
        "QPushButton {"
        "  background-color: #16213e;"
        "  color: #e0e0e0;"
        "  border: 2px solid #0f3460;"
        "  border-radius: 8px;"
        "  font-size: 20px;"
        "  padding: 12px 40px;"
        "  min-width: 200px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #0f3460;"
        "  border-color: #e94560;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #533483;"
        "}"
    );

    m_btnStart = new QPushButton(QStringLiteral("开始游戏"), this);
    m_btnStart->setStyleSheet(buttonStyle);
    m_btnStart->setCursor(Qt::PointingHandCursor);
    layout->addWidget(m_btnStart, 0, Qt::AlignCenter);

    layout->addSpacing(16);

    m_btnLoad = new QPushButton(QStringLiteral("读取游戏"), this);
    m_btnLoad->setStyleSheet(buttonStyle);
    m_btnLoad->setCursor(Qt::PointingHandCursor);
    layout->addWidget(m_btnLoad, 0, Qt::AlignCenter);

    layout->addSpacing(16);

    m_btnSettings = new QPushButton(QStringLiteral("设置"), this);
    m_btnSettings->setStyleSheet(buttonStyle);
    m_btnSettings->setCursor(Qt::PointingHandCursor);
    layout->addWidget(m_btnSettings, 0, Qt::AlignCenter);

    layout->addSpacing(16);

    m_btnExit = new QPushButton(QStringLiteral("退出游戏"), this);
    m_btnExit->setStyleSheet(buttonStyle);
    m_btnExit->setCursor(Qt::PointingHandCursor);
    layout->addWidget(m_btnExit, 0, Qt::AlignCenter);

    layout->addStretch(4);

    connect(m_btnStart, &QPushButton::clicked, this, &MainMenuPage::startGameClicked);
    connect(m_btnLoad, &QPushButton::clicked, this, &MainMenuPage::loadGameClicked);
    connect(m_btnSettings, &QPushButton::clicked, this, &MainMenuPage::settingsClicked);
    connect(m_btnExit, &QPushButton::clicked, this, &MainMenuPage::exitClicked);
}
