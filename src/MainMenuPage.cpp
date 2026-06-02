// =============================================================================
// MainMenuPage — title screen with logo, subtitle, and navigation buttons
// =============================================================================

#include "MainMenuPage.h"
#include "style.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QSpacerItem>

MainMenuPage::MainMenuPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void MainMenuPage::setupUI()
{
    auto *root = new QVBoxLayout(this);
    root->setAlignment(Qt::AlignCenter);

    // -- Logo area ------------------------------------------------------------
    root->addStretch(2);

    auto *logoIcon = new QLabel(QStringLiteral("🎒"), this);
    logoIcon->setAlignment(Qt::AlignCenter);
    logoIcon->setStyleSheet("font-size:72px; background:transparent;");
    root->addWidget(logoIcon, 0, Qt::AlignCenter);

    root->addSpacing(12);

    auto *title = new QLabel(QStringLiteral("末日迷途"), this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(QStringLiteral(
        "font-size:52px; font-weight:bold; color:%1; background:transparent;"
        "letter-spacing:12px;").arg(Style::kAccent));
    root->addWidget(title, 0, Qt::AlignCenter);

    auto *subtitle = new QLabel(QStringLiteral("废墟中，一只背包醒了过来"), this);
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setStyleSheet(QStringLiteral(
        "font-size:16px; color:%1; background:transparent; padding-bottom:8px;")
        .arg(Style::kTextDim));
    root->addWidget(subtitle, 0, Qt::AlignCenter);

    root->addSpacing(48);

    // -- Buttons --------------------------------------------------------------
    const QString btn = Style::kMenuBtn
        .arg(Style::kText, Style::kBorder, Style::kAccent, Style::kAccentP);

    auto makeBtn = [&](const QString &label, const QString &icon) -> QPushButton* {
        auto *b = new QPushButton(QString("%1  %2").arg(icon, label), this);
        b->setStyleSheet(btn);
        b->setCursor(Qt::PointingHandCursor);
        return b;
    };

    m_btnStart    = makeBtn(QStringLiteral("开始游戏"), "▶");
    m_btnLoad     = makeBtn(QStringLiteral("继续旅程"), "📂");
    m_btnSettings = makeBtn(QStringLiteral("设    置"), "⚙");
    m_btnExit     = makeBtn(QStringLiteral("离开废墟"), "🚪");

    const int btnSpacing = 14;
    root->addWidget(m_btnStart,    0, Qt::AlignCenter);
    root->addSpacing(btnSpacing);
    root->addWidget(m_btnLoad,     0, Qt::AlignCenter);
    root->addSpacing(btnSpacing);
    root->addWidget(m_btnSettings, 0, Qt::AlignCenter);
    root->addSpacing(btnSpacing);
    root->addWidget(m_btnExit,     0, Qt::AlignCenter);

    root->addStretch(3);

    // Version tag
    auto *ver = new QLabel(QStringLiteral("v0.8.0"), this);
    ver->setAlignment(Qt::AlignCenter);
    ver->setStyleSheet(QStringLiteral(
        "font-size:11px; color:%1; background:transparent;").arg(Style::kTextDim));
    root->addWidget(ver, 0, Qt::AlignCenter);
    root->addSpacing(12);

    connect(m_btnStart,    &QPushButton::clicked, this, &MainMenuPage::startGameClicked);
    connect(m_btnLoad,     &QPushButton::clicked, this, &MainMenuPage::loadGameClicked);
    connect(m_btnSettings, &QPushButton::clicked, this, &MainMenuPage::settingsClicked);
    connect(m_btnExit,     &QPushButton::clicked, this, &MainMenuPage::exitClicked);
}
