#include "GamePage.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QRandomGenerator>
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
    statsLayout->setSpacing(2);

    const QString statStyle = QStringLiteral(
        "QLabel { color: #e0e0e0; font-size: 16px; padding: 1px 0; }");

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
    m_restLabel = new QLabel(this);
    m_restLabel->setStyleSheet(statStyle);
    statsLayout->addWidget(m_restLabel);

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

    QPushButton *btnBigMap = new QPushButton(QStringLiteral("大地图"), this);
    btnBigMap->setStyleSheet(QStringLiteral(
        "QPushButton { background-color: #16213e; color: #e0e0e0;"
        " border: 2px solid #0f3460; border-radius: 8px;"
        " font-size: 16px; padding: 8px 24px; }"
        "QPushButton:hover { background-color: #0f3460;"
        " border-color: #e94560; }"));
    btnBigMap->setCursor(Qt::PointingHandCursor);
    connect(btnBigMap, &QPushButton::clicked, this, &GamePage::openBigMap);
    bottomLayout->addWidget(btnBigMap);

    QPushButton *btnSmallMap = new QPushButton(QStringLiteral("小地图"), this);
    btnSmallMap->setStyleSheet(btnBigMap->styleSheet());
    btnSmallMap->setCursor(Qt::PointingHandCursor);
    connect(btnSmallMap, &QPushButton::clicked, this, &GamePage::openSmallMap);
    bottomLayout->addWidget(btnSmallMap);

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

    QPushButton *btnMenu = new QPushButton(
        QStringLiteral("菜单"), this);
    btnMenu->setStyleSheet(QStringLiteral(
        "QPushButton { background-color: #16213e; color: #e0e0e0;"
        " border: 2px solid #0f3460; border-radius: 8px;"
        " font-size: 16px; padding: 8px 24px; }"
        "QPushButton:hover { background-color: #0f3460;"
        " border-color: #e94560; }"));
    btnMenu->setCursor(Qt::PointingHandCursor);
    connect(btnMenu, &QPushButton::clicked, this, &GamePage::openGameMenu);
    bottomLayout->addWidget(btnMenu);

    mainLayout->addLayout(bottomLayout);

    refreshStats();
}

double GamePage::statValue(StatChange::Target t) const
{
    switch (t) {
    case StatChange::Hp:      return m_stats.hp();
    case StatChange::Hunger:  return m_stats.hunger();
    case StatChange::Thirst:  return m_stats.thirst();
    case StatChange::Sanity:  return m_stats.sanity();
    case StatChange::Rest:    return m_stats.rest();
    }
    return 0;
}

bool GamePage::checkConditions(const Action &action)
{
    for (const auto &cond : action.conditions()) {
        double current = statValue(cond.target);
        if (current < cond.amount) {
            QMessageBox::warning(this,
                QStringLiteral("无法执行"),
                QStringLiteral("当前%1不足（需要 ≥%2，当前 %3）")
                    .arg(Action::targetName(cond.target))
                    .arg(cond.amount, 0, 'f', 0)
                    .arg(current, 0, 'f', 0));
            return false;
        }
    }
    for (const auto &cost : action.costs()) {
        double current = statValue(cost.target);
        if (current + cost.amount < 0) {
            QMessageBox::warning(this,
                QStringLiteral("无法执行"),
                QStringLiteral("当前%1不足").arg(Action::targetName(cost.target)));
            return false;
        }
    }
    return true;
}

void GamePage::processHourlyTicks(int minutesPassed)
{
    if (m_time.hour() < 0) {  // GameTime starts at hour 6 day 1, no tick yet
        // Only process ticks after actions actually advance time
    }
    int hours = minutesPassed / 60;
    for (int i = 0; i < hours; ++i)
        m_stats.applyHourlyTick();
}

void GamePage::setupTestActions()
{
    std::vector<Action> testActions = {
        Action(QStringLiteral("搜索"),
            QStringLiteral("搜索废墟中的物资"),
            {{StatChange::Hunger, -10}, {StatChange::Thirst, -10},
             {StatChange::Sanity, -10}},
            {},
            30,
            {},
            {
                {Item(QStringLiteral("木板"), "一块厚实的木板", 0.5), 100},
                {Item(QStringLiteral("木板"), "一块厚实的木板", 0.5), 50},
                {Item(QStringLiteral("面包"), "一块干硬的面包", 0.1,
                 {{StatChange::Hunger, 10}, {StatChange::Thirst, -5}}, true), 80},
            }),
        Action(QStringLiteral("等待"),
            QStringLiteral("在原地等待"),
            {},
            {},
            60),
        Action(QStringLiteral("休息"),
            QStringLiteral("在角落休息片刻"),
            {{StatChange::Hunger, -5}, {StatChange::Thirst, -3}},
            {{StatChange::Hp, 10}, {StatChange::Sanity, 5}},
            60),
        Action(QStringLiteral("睡觉"),
            QStringLiteral("好好睡一觉"),
            {},
            {{StatChange::Rest, 80}},
            480,
            {{StatChange::Hunger, 1}}),
        Action(QStringLiteral("跋涉"),
            QStringLiteral("向前推进一段距离"),
            {{StatChange::Hunger, -15}, {StatChange::Thirst, -20},
             {StatChange::Sanity, -5}},
            {},
            45,
            {{StatChange::Hunger, 1}})
    };

    for (const auto &action : testActions) {
        QPushButton *btn = createActionButton(action);
        m_actionLayout->addWidget(btn);
    }
}

void GamePage::setupTestItems()
{
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
    if (!checkConditions(action)) return;

    int oldDay = m_time.day();
    int oldHour = m_time.hour();

    // Process hourly ticks BEFORE applying costs (the time is about to advance)
    processHourlyTicks(action.timeCostMinutes());

    m_stats.applyChanges(action.costs());
    m_stats.applyChanges(action.yields());

    auto *rng = QRandomGenerator::global();
    for (const auto &entry : action.loot()) {
        if (rng->bounded(100) < entry.probability) {
            m_inventory->addItem(entry.item);
        }
    }

    m_time.advance(action.timeCostMinutes());
    refreshStats();

    if ((oldDay < m_time.day()) ||
        (oldDay == m_time.day() && oldHour < 6 && m_time.hour() >= 6)) {
        emit autoSaveTriggered();
    }
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
    m_restLabel->setText(m_stats.restString());
    m_timeLabel->setText(m_time.displayString());
}
