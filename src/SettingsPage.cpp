#include "SettingsPage.h"
#include <QCheckBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

SettingsPage::SettingsPage(QWidget *parent) : QWidget(parent) { setupUI(); }

void SettingsPage::setupUI()
{
    QVBoxLayout *ml = new QVBoxLayout(this);
    ml->setContentsMargins(40, 40, 40, 40);

    QLabel *title = new QLabel(QStringLiteral("设置"), this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(QStringLiteral(
        "QLabel{color:#e94560;font-size:28px;font-weight:bold;padding:8px}"));
    ml->addWidget(title);
    ml->addSpacing(30);

    // Resolution row
    QHBoxLayout *resLayout = new QHBoxLayout();
    QLabel *resLabel = new QLabel(QStringLiteral("分辨率"), this);
    resLabel->setStyleSheet(QStringLiteral(
        "QLabel{color:#e0e0e0;font-size:18px}"));
    resLayout->addWidget(resLabel);
    resLayout->addStretch();

    m_resolutionCombo = new QComboBox(this);
    m_resolutionCombo->addItem(QStringLiteral("1280x720"));
    m_resolutionCombo->addItem(QStringLiteral("1366x768"));
    m_resolutionCombo->addItem(QStringLiteral("1600x900"));
    m_resolutionCombo->addItem(QStringLiteral("1920x1080"));
    m_resolutionCombo->setCurrentIndex(1); // Default 1366x768
    m_resolutionCombo->setStyleSheet(QStringLiteral(
        "QComboBox{background-color:#16213e;color:#e0e0e0;"
        "border:1px solid #0f3460;border-radius:4px;"
        "font-size:16px;padding:6px 12px;min-width:140px}"
        "QComboBox:hover{border-color:#e94560}"
        "QComboBox QAbstractItemView{background-color:#16213e;"
        "color:#e0e0e0;selection-background-color:#0f3460}"));
    resLayout->addWidget(m_resolutionCombo);
    ml->addLayout(resLayout);
    ml->addSpacing(16);

    // Fullscreen row
    QHBoxLayout *fsLayout = new QHBoxLayout();
    QLabel *fsLabel = new QLabel(QStringLiteral("全屏"), this);
    fsLabel->setStyleSheet(QStringLiteral(
        "QLabel{color:#e0e0e0;font-size:18px}"));
    fsLayout->addWidget(fsLabel);
    fsLayout->addStretch();

    m_fullscreenCheck = new QCheckBox(this);
    m_fullscreenCheck->setStyleSheet(QStringLiteral(
        "QCheckBox{color:#e0e0e0;font-size:16px}"
        "QCheckBox::indicator{width:24px;height:24px;"
        "border:2px solid #0f3460;border-radius:4px;"
        "background-color:#16213e}"
        "QCheckBox::indicator:checked{background-color:#e94560;"
        "border-color:#e94560}"));
    fsLayout->addWidget(m_fullscreenCheck);
    ml->addLayout(fsLayout);
    ml->addStretch();

    // Buttons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();

    QPushButton *btnApply = new QPushButton(QStringLiteral("应用"), this);
    btnApply->setStyleSheet(QStringLiteral(
        "QPushButton{background-color:#e94560;color:#ffffff;"
        "border:2px solid #e94560;border-radius:8px;"
        "font-size:16px;padding:8px 24px}"
        "QPushButton:hover{background-color:#ff6b81}"));
    btnApply->setCursor(Qt::PointingHandCursor);
    connect(btnApply, &QPushButton::clicked, this, &SettingsPage::applySettings);
    btnLayout->addWidget(btnApply);
    btnLayout->addSpacing(16);

    QPushButton *btnBack = new QPushButton(QStringLiteral("返回"), this);
    btnBack->setStyleSheet(QStringLiteral(
        "QPushButton{background-color:#16213e;color:#e0e0e0;"
        "border:2px solid #0f3460;border-radius:8px;"
        "font-size:16px;padding:8px 24px}"
        "QPushButton:hover{background-color:#0f3460;border-color:#e94560}"));
    btnBack->setCursor(Qt::PointingHandCursor);
    connect(btnBack, &QPushButton::clicked, this, &SettingsPage::backClicked);
    btnLayout->addWidget(btnBack);

    ml->addLayout(btnLayout);
}

void SettingsPage::applySettings()
{
    QString res = m_resolutionCombo->currentText();
    QStringList parts = res.split('x');
    if (parts.size() == 2) {
        int w = parts[0].toInt();
        int h = parts[1].toInt();
        emit resolutionChanged(w, h);
    }
    emit fullscreenToggled(m_fullscreenCheck->isChecked());
}
