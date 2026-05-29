#include "LoadingPage.h"

#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>

LoadingPage::LoadingPage(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    QLabel *label = new QLabel(QStringLiteral("加载中..."), this);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet(QStringLiteral(
        "QLabel {"
        "  color: #e0e0e0;"
        "  font-size: 24px;"
        "  padding: 10px;"
        "}"
    ));
    layout->addWidget(label);

    layout->addSpacing(20);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setTextVisible(false);
    m_progressBar->setFixedHeight(8);
    m_progressBar->setFixedWidth(400);
    m_progressBar->setStyleSheet(QStringLiteral(
        "QProgressBar {"
        "  background-color: #16213e;"
        "  border: none;"
        "  border-radius: 4px;"
        "}"
        "QProgressBar::chunk {"
        "  background-color: #e94560;"
        "  border-radius: 4px;"
        "}"
    ));
    layout->addWidget(m_progressBar, 0, Qt::AlignCenter);

    // 模拟加载动画
    m_progressBar->setValue(35);
}
