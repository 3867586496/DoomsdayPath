#include "GamePage.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

GamePage::GamePage(QWidget *parent)
    : QWidget(parent)
{
    m_inventory = new Inventory(this);
    setupUI();
    setupTestActions();
    setupTestItems();
}

void GamePage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 16, 20, 16);

    QHBoxLayout *topBar = new QHBoxLayout();
    QVBoxLayout *statsLayout = new QVBoxLayout();
    statsLayout->setSpacing(4);

    const QString statStyle = QStringLiteral(
        "QLabel { color: #e0e0e0; font-size: 18px; padding: 2px 0; }");

    m_hpLabel = new QLabel(this);
    m_hpLabel->setStyleSheet(statStyle);
    statsLayout->addWidget(m_hpLabel);
    m_hungerLabel = new QLabel(this);
    m_hungerLabel->setStyleSheet(statStyle);
    statsLayout->addWidget(m_hungerLabel);
    m_thirstLabel = new QLabel(this);
    m_thirstLabel->setStyleSheet(statStyle);
    statsLayout->addWidget(m_thirstLabel);
    m_sanityLabel = new QLabel(this);
    m_sanityLabel->setStyleSheet(statStyle);
    statsLayout->addWidget(m_sanityLabel);

    topBar->addLayout(statsLayout);
    topBar->addStretch();

    m_timeLabel = new QLabel(this);
    m_timeLabel->setAlignment(Qt::AlignRight | Qt::AlignTop);
    m_timeLabel->setStyleSheet(QStringLiteral(
        "QLabel { color: #a0a0b0; font-size: 16px; }"));
    topBar->addWidget(m_timeLabel);
    mainLayout->addLayout(topBar);

    QLabel *separator = new QLabel(this);
    separator->setFixedHeight(2);
    separator->setStyleSheet(QStringLiteral(
        "QLabel { background-color: #0f3460; }"));
    mainLayout->addWidget(separator);
    mainLayout->addSpacing(10);
    mainLayout->addStretch(3);

    m_testArea = new QWidget(this);
    QVBoxLayout *testLayout = new QVBoxLayout(m_testArea);
    testLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *testLabel = new QLabel(
        QStringLiteral("[测试区 - 正式版删除]"), this);
    testLabel->setAlignment(Qt::AlignCenter);
    testLabel->setStyleSheet(QStringLiteral(
        "QLabel { color: #888800; font-size: 14px;"
        " font-style: italic; padding: 4px; }"));
    testLayout->addWidget(testLabel);
    testLayout->addSpacing(4);

    m_actionLayout = new QVBoxLayout();
    m_actionLayout->setSpacing(6);
    testLayout->addLayout(m_actionLayout);

    QPushButton *btnBackpack = new QPushButton(
        QStringLiteral("[背包] 打开背包"), this);
    btnBackpack->setStyleSheet(QStringLiteral(
        "QPushButton { background-color: #16213e; color: #c0c0d0;"
        " border: 1px solid #0f3460; border-radius: 6px;"
        " font-size: 14px; padding: 8px 16px; text-align: left; }"
        "QPushButton:hover { background-color: #1a2744;"
        " border-color: #e94560; color: #ffffff; }"));
    btnBackpack->setCursor(Qt::PointingHandCursor);
    connect(btnBackpack, &QPushButton::clicked,
            this, &GamePage::onBackpackClicked);
    testLayout->addWidget(btnBackpack);
    mainLayout->addWidget(m_testArea);

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();

    QPushButton *btnBack = new QPushButton(
        QStringLiteral("返回主菜单"), this);
    btnBack->setStyleSheet(QStringLiteral(
        "QPushButton { background-color: #16213e; color: #e0e0e0;"
        " border: 2px solid #0f3460; border-radius: 8px;"
        " font-size: 16px; padding: 8px 24px; }"
        "QPushButton:hover { background-color: #0f3460;"
        " border-color: #e94560; }"));
    btnBack->setCursor(Qt::PointingHandCursor);
    connect(btnBack, &QPushButton::clicked, this, &GamePage::backToMenu);
    bottomLayout->addWidget(btnBack);
    mainLayout->addLayout(bottomLayout);

    refreshStats();
}

void GamePage::setupTestActions()
{
    std::vector<Action> testActions = {
        Action(QStringLiteral("搜索"),
            "搜索废墟中的物资",
            {{StatChange::Hunger, -10}, {StatChange::Thirst, -5}},
            {{StatChange::Hunger, 15}, {StatChange::Thirst, 10}}, 30),
        Action(QStringLiteral("休息"),
            "在角落休息片刻",
            {{StatChange::Hunger, -5}, {StatChange::Thirst, -3}},
            {{StatChange::Hp, 10}, {StatChange::Sanity, 5}}, 60),
        Action(QStringLiteral("跋涉"),
            "向前推进一段距离",
            {{StatChange::Hunger, -15}, {StatChange::Thirst, -20},
             {StatChange::Sanity, -5}}, {}, 45),
    };

    for (const auto &action : testActions) {
        QPushButton *btn = createActionButton(action);
        m_actionLayout->addWidget(btn);
    }
}

void GamePage::setupTestItems()
{
    // Design-specified test items
    m_inventory->addItem(Item(QStringLiteral("面包"),
        "一块干硬的面包", 0.1,
        {{StatChange::Hunger, 10}, {StatChange::Thirst, -5}}, true));
    m_inventory->addItem(Item(QStringLiteral("木板"),
        "一块厚实的木板", 0.5));
}

QPushButton *GamePage::createActionButton(const Action &action)
{
    QString labelText = QStringLiteral("[%1分钟] %2")
        .arg(action.timeCostMinutes()).arg(action.name());

    QPushButton *btn = new QPushButton(labelText, this);
    btn->setStyleSheet(QStringLiteral(
        "QPushButton { background-color: #16213e; color: #c0c0d0;"
        " border: 1px solid #0f3460; border-radius: 6px;"
        " font-size: 14px; padding: 8px 16px; text-align: left; }"
        "QPushButton:hover { background-color: #1a2744;"
        " border-color: #e94560; color: #ffffff; }"));
    btn->setCursor(Qt::PointingHandCursor);

    connect(btn, &QPushButton::clicked, this,
            [this, action]() { onActionClicked(action); });

    return btn;
}

void GamePage::onActionClicked(const Action &action)
{
    m_stats.applyChanges(action.costs());
    m_stats.applyChanges(action.yields());
    m_time.advance(action.timeCostMinutes());
    refreshStats();
}

void GamePage::onBackpackClicked() { emit openBackpack(); }

void GamePage::applyItemEffects(const std::vector<StatChange> &effects)
{
    m_stats.applyChanges(effects);
    refreshStats();
}

void GamePage::refreshStats()
{
    m_hpLabel->setText(m_stats.hpString());
    m_hungerLabel->setText(m_stats.hungerString());
    m_thirstLabel->setText(m_stats.thirstString());
    m_sanityLabel->setText(m_stats.sanityString());
    m_timeLabel->setText(m_time.displayString());
}
