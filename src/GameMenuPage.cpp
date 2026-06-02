// =============================================================================
// GameMenuPage — in-game pause overlay
// =============================================================================

#include "GameMenuPage.h"
#include "style.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

GameMenuPage::GameMenuPage(QWidget *parent) : QWidget(parent) { setupUI(); }

void GameMenuPage::setupUI()
{
    auto *ml = new QVBoxLayout(this);
    ml->setContentsMargins(40, 60, 40, 60);
    ml->setAlignment(Qt::AlignCenter);

    ml->addStretch(2);

    auto *title = new QLabel(QStringLiteral("⏸  游戏暂停"), this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(QStringLiteral(
        "QLabel{color:%1;font-size:36px;font-weight:bold;padding:16px}")
        .arg(Style::kAccent));
    ml->addWidget(title);
    ml->addSpacing(40);

    const QString btn = Style::kMenuBtn
        .arg(Style::kText, Style::kBorder, Style::kAccent, Style::kAccentP);

    auto makeBtn = [&](const QString &label, const QString &icon) -> QPushButton* {
        auto *b = new QPushButton(QString("%1  %2").arg(icon, label), this);
        b->setStyleSheet(btn);
        b->setCursor(Qt::PointingHandCursor);
        return b;
    };

    auto *btnResume = makeBtn(QStringLiteral("继续废墟之旅"), "▶");
    connect(btnResume, &QPushButton::clicked, this, &GameMenuPage::resumeClicked);
    ml->addWidget(btnResume, 0, Qt::AlignCenter);
    ml->addSpacing(14);

    auto *btnSave = makeBtn(QStringLiteral("保存进度"), "💾");
    connect(btnSave, &QPushButton::clicked, this, &GameMenuPage::saveClicked);
    ml->addWidget(btnSave, 0, Qt::AlignCenter);
    ml->addSpacing(14);

    auto *btnExit = makeBtn(QStringLiteral("返回主菜单"), "🚪");
    connect(btnExit, &QPushButton::clicked, this, &GameMenuPage::exitToMainMenu);
    ml->addWidget(btnExit, 0, Qt::AlignCenter);

    ml->addStretch(3);
}
