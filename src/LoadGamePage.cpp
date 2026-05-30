#include "LoadGamePage.h"
#include "SaveSystem.h"

#include <QDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

LoadGamePage::LoadGamePage(SaveSystem *saveSystem, QWidget *parent)
    : QWidget(parent), m_saveSystem(saveSystem)
{
    setupUI();
    connect(m_saveSystem, &SaveSystem::savesChanged, this, &LoadGamePage::refreshList);
}

void LoadGamePage::setupUI()
{
    QVBoxLayout *ml = new QVBoxLayout(this);
    ml->setContentsMargins(12, 12, 12, 12);

    QLabel *title = new QLabel(QStringLiteral("加载游戏"), this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(QStringLiteral(
        "QLabel{color:#e94560;font-size:28px;font-weight:bold;padding:8px}"));
    ml->addWidget(title);
    ml->addSpacing(8);

    m_table = new QTableWidget(0, 3, this);
    m_table->setHorizontalHeaderLabels({
        QStringLiteral("名称"),
        QStringLiteral("最后保存"),
        QStringLiteral("操作")
    });
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->verticalHeader()->setVisible(false);
    m_table->verticalHeader()->setDefaultSectionSize(64);
    m_table->setShowGrid(true);
    m_table->setAlternatingRowColors(true);
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_table->horizontalHeader()->resizeSection(0, 120);
    m_table->horizontalHeader()->resizeSection(1, 150);
    m_table->setStyleSheet(QStringLiteral(
        "QTableWidget{background-color:#1a1a2e;border:1px solid #0f3460;"
        "gridline-color:#0f3460;color:#e0e0e0;font-size:15px}"
        "QTableWidget::item{padding:10px 12px}"
        "QTableWidget::item:alternate{background-color:#162040}"
        "QHeaderView::section{background-color:#0f3460;color:#a0a0b0;"
        "font-size:14px;font-weight:bold;padding:8px 12px;"
        "border:none;border-bottom:2px solid #1a1a2e}"));
    ml->addWidget(m_table);
    ml->addSpacing(16);

    QHBoxLayout *bl = new QHBoxLayout();
    bl->addStretch();
    m_btnBack = new QPushButton(QStringLiteral("返回"), this);
    m_btnBack->setStyleSheet(QStringLiteral(
        "QPushButton{background-color:#16213e;color:#e0e0e0;"
        "border:2px solid #0f3460;border-radius:8px;"
        "font-size:16px;padding:8px 24px}"
        "QPushButton:hover{background-color:#0f3460;border-color:#e94560}"));
    m_btnBack->setCursor(Qt::PointingHandCursor);
    connect(m_btnBack, &QPushButton::clicked, this, &LoadGamePage::backClicked);
    bl->addWidget(m_btnBack);
    ml->addLayout(bl);

    refreshList();
}

void LoadGamePage::refreshList()
{
    QStringList folders = m_saveSystem->saveFolderNames();
    m_table->setRowCount(0);
    m_table->setRowCount(folders.size());
    for (int i = 0; i < folders.size(); ++i) {
        populateFolderRow(i, folders[i]);
        m_table->setRowHeight(i, 64);
    }
}

void LoadGamePage::populateFolderRow(int row, const QString &folderName)
{
    auto latest = m_saveSystem->latestEntry(folderName);
    QString cellStyle = QStringLiteral(
        "QLabel{color:#e0e0e0;font-size:15px;padding:4px 8px}");
    QString btnStyle = QStringLiteral(
        "QPushButton{background-color:#16213e;color:#c0c0d0;"
        "border:1px solid #0f3460;border-radius:4px;"
        "font-size:13px;padding:4px 12px;min-height:28px}"
        "QPushButton:hover{background-color:#1a2744;"
        "border-color:#e94560;color:#fff}");

    QLabel *nl = new QLabel(folderName);
    nl->setStyleSheet(cellStyle);
    m_table->setCellWidget(row, 0, nl);

    QString timeStr = latest.timestamp.isValid()
        ? latest.timestamp.toString("MM-dd HH:mm")
        : QStringLiteral("-");
    QLabel *tl = new QLabel(timeStr);
    tl->setStyleSheet(cellStyle);
    m_table->setCellWidget(row, 1, tl);

    QWidget *aw = new QWidget();
    QHBoxLayout *al = new QHBoxLayout(aw);
    al->setContentsMargins(4, 8, 4, 8);
    al->setSpacing(8);

    QPushButton *btnLoad = new QPushButton(QStringLiteral("加载"));
    btnLoad->setStyleSheet(QStringLiteral(
        "QPushButton{background-color:#e94560;color:#fff;"
        "border:1px solid #e94560;border-radius:4px;"
        "font-size:13px;padding:4px 12px;min-height:28px}"
        "QPushButton:hover{background-color:#ff6b81}"));
    btnLoad->setCursor(Qt::PointingHandCursor);
    connect(btnLoad, &QPushButton::clicked, this,
            [this, folderName]() { onLoadFolder(folderName); });
    al->addWidget(btnLoad);

    QPushButton *btnDetail = new QPushButton(QStringLiteral("详情"));
    btnDetail->setStyleSheet(btnStyle);
    btnDetail->setCursor(Qt::PointingHandCursor);
    connect(btnDetail, &QPushButton::clicked, this,
            [this, folderName]() { onShowDetails(folderName); });
    al->addWidget(btnDetail);

    QPushButton *btnDel = new QPushButton(QStringLiteral("删除"));
    btnDel->setStyleSheet(btnStyle);
    btnDel->setCursor(Qt::PointingHandCursor);
    connect(btnDel, &QPushButton::clicked, this,
            [this, folderName]() { onDeleteFolder(folderName); });
    al->addWidget(btnDel);

    m_table->setCellWidget(row, 2, aw);
}

void LoadGamePage::onDeleteFolder(const QString &folderName)
{
    auto reply = QMessageBox::question(this,
        QStringLiteral("确认删除"),
        QStringLiteral("确定要删除存档「%1」吗？此操作不可撤销。").arg(folderName),
        QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        m_saveSystem->deleteFolder(folderName);
    }
}

void LoadGamePage::onLoadFolder(const QString &folderName)
{
    auto entries = m_saveSystem->entriesInFolder(folderName);
    if (entries.empty()) return;
    int latestIdx = 0;
    for (int i = 1; i < static_cast<int>(entries.size()); ++i) {
        if (entries[i].timestamp > entries[latestIdx].timestamp)
            latestIdx = i;
    }
    emit loadRequested(folderName, latestIdx);
}

void LoadGamePage::onShowDetails(const QString &folderName)
{
    auto entries = m_saveSystem->entriesInFolder(folderName);
    if (entries.empty()) {
        QMessageBox::information(this, QStringLiteral("详情"),
            QStringLiteral("该存档下没有保存记录"));
        return;
    }

    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle(QStringLiteral("存档详情 - %1").arg(folderName));
    dialog->resize(600, 400);
    dialog->setStyleSheet(QStringLiteral(
        "QDialog{background-color:#1a1a2e}"));

    QVBoxLayout *dl = new QVBoxLayout(dialog);
    QTableWidget *dt = new QTableWidget(entries.size(), 3, dialog);
    dt->setHorizontalHeaderLabels({
        QStringLiteral("时间"), QStringLiteral("天数"), QStringLiteral("操作")
    });
    dt->setEditTriggers(QAbstractItemView::NoEditTriggers);
    dt->verticalHeader()->setVisible(false);
    dt->verticalHeader()->setDefaultSectionSize(40);
    dt->setShowGrid(false);
    dt->setAlternatingRowColors(true);
    dt->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    dt->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    dt->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    dt->setStyleSheet(QStringLiteral(
        "QTableWidget{background-color:#1a1a2e;border:1px solid #0f3460;"
        "gridline-color:#0f3460;color:#e0e0e0;font-size:14px}"
        "QTableWidget::item{padding:8px 12px}"
        "QHeaderView::section{background-color:#0f3460;color:#a0a0b0;"
        "font-size:13px;font-weight:bold;padding:6px 12px;"
        "border:none;border-bottom:2px solid #1a1a2e}"));

    QString cellStyle = QStringLiteral(
        "QLabel{color:#e0e0e0;font-size:14px;padding:4px 8px}");
    QString btnStyle = QStringLiteral(
        "QPushButton{background-color:#16213e;color:#c0c0d0;"
        "border:1px solid #0f3460;border-radius:4px;"
        "font-size:12px;padding:3px 10px;min-height:28px}"
        "QPushButton:hover{background-color:#e94560;color:#fff}");

    for (int i = 0; i < static_cast<int>(entries.size()); ++i) {
        QLabel *tl = new QLabel(entries[i].timestamp.toString("MM-dd HH:mm:ss"));
        tl->setStyleSheet(cellStyle);
        dt->setCellWidget(i, 0, tl);

        QString dayStr = entries[i].isAuto
            ? QStringLiteral("第%1天(自动)").arg(entries[i].day)
            : QStringLiteral("第%1天(手动)").arg(entries[i].day);
        QLabel *dl2 = new QLabel(dayStr);
        dl2->setStyleSheet(cellStyle);
        dt->setCellWidget(i, 1, dl2);

        QWidget *aw = new QWidget();
        QHBoxLayout *al = new QHBoxLayout(aw);
        al->setContentsMargins(2, 2, 2, 2); al->setSpacing(4);

        QPushButton *bload = new QPushButton(QStringLiteral("加载"));
        bload->setStyleSheet(btnStyle);
        bload->setCursor(Qt::PointingHandCursor);
        int idx = i;
        connect(bload, &QPushButton::clicked, this, [this, folderName, idx, dialog]() {
            dialog->accept();
            emit loadRequested(folderName, idx);
        });
        al->addWidget(bload);

        QPushButton *bdel = new QPushButton(QStringLiteral("删除"));
        bdel->setStyleSheet(btnStyle);
        bdel->setCursor(Qt::PointingHandCursor);
        connect(bdel, &QPushButton::clicked, this, [this, folderName, idx, dt]() {
            auto r = QMessageBox::question(dt, QStringLiteral("确认"),
                QStringLiteral("删除此记录？"));
            if (r == QMessageBox::Yes)
                m_saveSystem->deleteEntry(folderName, idx);
        });
        al->addWidget(bdel);

        dt->setCellWidget(i, 2, aw);
        dt->setRowHeight(i, 40);
    }
    dl->addWidget(dt);

    QPushButton *closeBtn = new QPushButton(QStringLiteral("关闭"), dialog);
    closeBtn->setStyleSheet(QStringLiteral(
        "QPushButton{background-color:#16213e;color:#e0e0e0;"
        "border:2px solid #0f3460;border-radius:6px;"
        "font-size:14px;padding:6px 24px}"
        "QPushButton:hover{background-color:#0f3460}"));
    connect(closeBtn, &QPushButton::clicked, dialog, &QDialog::accept);
    dl->addWidget(closeBtn, 0, Qt::AlignCenter);

    connect(m_saveSystem, &SaveSystem::savesChanged, dialog, &QDialog::accept);
    dialog->exec();
    dialog->deleteLater();
    refreshList();
}
