#include "NewGamePage.h"

#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>

NewGamePage::NewGamePage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void NewGamePage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);

    mainLayout->addStretch(2);

    // —— 顶部标题 ——
    QLabel *titleLabel = new QLabel(QStringLiteral("新的旅途"), this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(QStringLiteral(
        "QLabel {"
        "  color: #e94560;"
        "  font-size: 36px;"
        "  font-weight: bold;"
        "  padding: 10px;"
        "}"
    ));
    mainLayout->addWidget(titleLabel);

    mainLayout->addSpacing(30);

    // —— 存档名称 ——
    QLabel *nameLabel = new QLabel(QStringLiteral("存档名称"), this);
    nameLabel->setAlignment(Qt::AlignCenter);
    nameLabel->setStyleSheet(QStringLiteral(
        "QLabel { color: #a0a0b0; font-size: 16px; }"
    ));
    mainLayout->addWidget(nameLabel);

    m_saveNameEdit = new QLineEdit(this);
    m_saveNameEdit->setPlaceholderText(QStringLiteral("输入存档名称..."));
    m_saveNameEdit->setAlignment(Qt::AlignCenter);
    m_saveNameEdit->setMaxLength(32);
    m_saveNameEdit->setStyleSheet(QStringLiteral(
        "QLineEdit {"
        "  background-color: #16213e;"
        "  color: #e0e0e0;"
        "  border: 2px solid #0f3460;"
        "  border-radius: 6px;"
        "  font-size: 18px;"
        "  padding: 10px 20px;"
        "  max-width: 400px;"
        "}"
        "QLineEdit:focus {"
        "  border-color: #e94560;"
        "}"
    ));
    mainLayout->addWidget(m_saveNameEdit, 0, Qt::AlignCenter);

    mainLayout->addSpacing(30);

    // —— 难度选择 ——
    QLabel *diffLabel = new QLabel(QStringLiteral("难度选择"), this);
    diffLabel->setAlignment(Qt::AlignCenter);
    diffLabel->setStyleSheet(QStringLiteral(
        "QLabel { color: #a0a0b0; font-size: 16px; }"
    ));
    mainLayout->addWidget(diffLabel);

    mainLayout->addSpacing(10);

    const QString radioStyle = QStringLiteral(
        "QRadioButton {"
        "  color: #c0c0d0;"
        "  font-size: 16px;"
        "  spacing: 8px;"
        "  padding: 6px 12px;"
        "}"
        "QRadioButton::indicator {"
        "  width: 18px;"
        "  height: 18px;"
        "}"
        "QRadioButton:hover {"
        "  color: #e94560;"
        "}"
    );

    QHBoxLayout *presetLayout = new QHBoxLayout();
    presetLayout->setAlignment(Qt::AlignCenter);
    presetLayout->setSpacing(24);

    m_rbEasy = new QRadioButton(QStringLiteral("简单 ×0.5"), this);
    m_rbEasy->setStyleSheet(radioStyle);
    m_rbEasy->setCursor(Qt::PointingHandCursor);
    presetLayout->addWidget(m_rbEasy);

    m_rbNormal = new QRadioButton(QStringLiteral("正常 ×1.0"), this);
    m_rbNormal->setStyleSheet(radioStyle);
    m_rbNormal->setCursor(Qt::PointingHandCursor);
    m_rbNormal->setChecked(true);
    presetLayout->addWidget(m_rbNormal);

    m_rbHard = new QRadioButton(QStringLiteral("困难 ×2.0"), this);
    m_rbHard->setStyleSheet(radioStyle);
    m_rbHard->setCursor(Qt::PointingHandCursor);
    presetLayout->addWidget(m_rbHard);

    m_rbHell = new QRadioButton(QStringLiteral("地狱 ×5.0"), this);
    m_rbHell->setStyleSheet(radioStyle);
    m_rbHell->setCursor(Qt::PointingHandCursor);
    presetLayout->addWidget(m_rbHell);

    mainLayout->addLayout(presetLayout);

    // 自定义难度
    QHBoxLayout *customLayout = new QHBoxLayout();
    customLayout->setAlignment(Qt::AlignCenter);

    m_rbCustom = new QRadioButton(QStringLiteral("自定义"), this);
    m_rbCustom->setStyleSheet(radioStyle);
    m_rbCustom->setCursor(Qt::PointingHandCursor);
    customLayout->addWidget(m_rbCustom);

    m_customFactorSpin = new QDoubleSpinBox(this);
    m_customFactorSpin->setRange(0.0, 100.0);
    m_customFactorSpin->setValue(1.0);
    m_customFactorSpin->setSingleStep(0.1);
    m_customFactorSpin->setDecimals(2);
    m_customFactorSpin->setSuffix(QStringLiteral(" 倍"));
    m_customFactorSpin->setEnabled(false);
    m_customFactorSpin->setStyleSheet(QStringLiteral(
        "QDoubleSpinBox {"
        "  background-color: #16213e;"
        "  color: #e0e0e0;"
        "  border: 2px solid #0f3460;"
        "  border-radius: 6px;"
        "  font-size: 16px;"
        "  padding: 6px 12px;"
        "}"
        "QDoubleSpinBox:focus {"
        "  border-color: #e94560;"
        "}"
        "QDoubleSpinBox:disabled {"
        "  background-color: #0d1528;"
        "  color: #555566;"
        "  border-color: #1a1a2e;"
        "}"
    ));
    customLayout->addWidget(m_customFactorSpin);

    mainLayout->addLayout(customLayout);

    connect(m_rbEasy, &QRadioButton::toggled, this, &NewGamePage::onPresetToggled);
    connect(m_rbNormal, &QRadioButton::toggled, this, &NewGamePage::onPresetToggled);
    connect(m_rbHard, &QRadioButton::toggled, this, &NewGamePage::onPresetToggled);
    connect(m_rbHell, &QRadioButton::toggled, this, &NewGamePage::onPresetToggled);
    connect(m_rbCustom, &QRadioButton::toggled, this, &NewGamePage::onPresetToggled);

    // —— 预留空间 ——
    mainLayout->addStretch(2);

    // —— 底部按钮 ——
    const QString bottomBtnStyle = QStringLiteral(
        "QPushButton {"
        "  background-color: #16213e;"
        "  color: #e0e0e0;"
        "  border: 2px solid #0f3460;"
        "  border-radius: 8px;"
        "  font-size: 18px;"
        "  padding: 10px 30px;"
        "  min-width: 160px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #0f3460;"
        "  border-color: #e94560;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #533483;"
        "}"
    );

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->setAlignment(Qt::AlignCenter);
    bottomLayout->setSpacing(40);

    m_btnBack = new QPushButton(QStringLiteral("返回"), this);
    m_btnBack->setStyleSheet(bottomBtnStyle);
    m_btnBack->setCursor(Qt::PointingHandCursor);
    bottomLayout->addWidget(m_btnBack);

    m_btnStart = new QPushButton(QStringLiteral("开始游戏"), this);
    m_btnStart->setStyleSheet(QStringLiteral(
        "QPushButton {"
        "  background-color: #e94560;"
        "  color: #ffffff;"
        "  border: none;"
        "  border-radius: 8px;"
        "  font-size: 18px;"
        "  font-weight: bold;"
        "  padding: 10px 30px;"
        "  min-width: 160px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #ff6b81;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #c0392b;"
        "}"
    ));
    m_btnStart->setCursor(Qt::PointingHandCursor);
    bottomLayout->addWidget(m_btnStart);

    mainLayout->addLayout(bottomLayout);

    mainLayout->addStretch(2);

    connect(m_btnBack, &QPushButton::clicked, this, &NewGamePage::backClicked);
    connect(m_btnStart, &QPushButton::clicked, this, [this]() {
        DifficultyFactor df = difficulty();
        emit startGame(m_saveNameEdit->text().trimmed(), df);
    });
}

void NewGamePage::onPresetToggled()
{
    m_customFactorSpin->setEnabled(m_rbCustom->isChecked());
}

QString NewGamePage::saveName() const
{
    return m_saveNameEdit->text().trimmed();
}

DifficultyFactor NewGamePage::difficulty() const
{
    if (m_rbEasy->isChecked())
        return DifficultyFactor(DifficultyFactor::presetEasy().factor);
    if (m_rbNormal->isChecked())
        return DifficultyFactor(DifficultyFactor::presetNormal().factor);
    if (m_rbHard->isChecked())
        return DifficultyFactor(DifficultyFactor::presetHard().factor);
    if (m_rbHell->isChecked())
        return DifficultyFactor(DifficultyFactor::presetHell().factor);
    // custom
    return DifficultyFactor(m_customFactorSpin->value());
}
