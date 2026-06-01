#include "SavePage.h"
#include "SaveSystem.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

static const QString kCellStyle = QStringLiteral(
    "QLabel{color:#e0e0e0;font-size:14px;padding:4px 8px}");
static const QString kBtnStyle = QStringLiteral(
    "QPushButton{background-color:#16213e;color:#c0c0d0;"
    "border:1px solid #0f3460;border-radius:4px;"
    "font-size:13px;padding:4px 12px;min-height:28px}"
    "QPushButton:hover{background-color:#1a2744;"
    "border-color:#e94560;color:#fff}");

SavePage::SavePage(SaveSystem *saveSystem, QWidget *parent)
    : QWidget(parent), m_saveSystem(saveSystem)
{
    setupUI();
}

void SavePage::setupUI()
{
    QVBoxLayout *ml = new QVBoxLayout(this);
    ml->setContentsMargins(16, 16, 16, 16);

    QLabel *title = new QLabel(QStringLiteral("存档"), this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(QStringLiteral(
        "QLabel{color:#e94560;font-size:24px;font-weight:bold;padding:8px}"));
    ml->addWidget(title);
    ml->addSpacing(12);

    m_table = new QTableWidget(0, 3, this);
    m_table->setHorizontalHeaderLabels({
        QStringLiteral("名称"),
        QStringLiteral("日期"),
        QStringLiteral("功能")
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
    m_table->horizontalHeader()->resizeSection(1, 160);
    m_table->setStyleSheet(QStringLiteral(
        "QTableWidget{background-color:#1a1a2e;border:1px solid #0f3460;"
        "gridline-color:#0f3460;color:#e0e0e0;font-size:15px}"
        "QTableWidget::item{padding:12px 10px}"
        "QTableWidget::item:alternate{background-color:#162040}"
        "QHeaderView::section{background-color:#0f3460;color:#a0a0b0;"
        "font-size:14px;font-weight:bold;padding:8px 12px;"
        "border:none;border-bottom:2px solid #1a1a2e}"));
    ml->addWidget(m_table);
    ml->addSpacing(16);

    QHBoxLayout *bl = new QHBoxLayout();
    QPushButton *btnNew = new QPushButton(QStringLiteral("新建"), this);
    btnNew->setStyleSheet(QStringLiteral(
        "QPushButton{background-color:#e94560;color:#fff;"
        "border:2px solid #e94560;border-radius:8px;"
        "font-size:16px;padding:8px 24px}"
        "QPushButton:hover{background-color:#ff6b81}"));
    btnNew->setCursor(Qt::PointingHandCursor);
    connect(btnNew, &QPushButton::clicked, this, &SavePage::onNewSave);
    bl->addWidget(btnNew);
    bl->addStretch();

    QPushButton *btnCancel = new QPushButton(QStringLiteral("取消"), this);
    btnCancel->setStyleSheet(QStringLiteral(
        "QPushButton{background-color:#16213e;color:#e0e0e0;"
        "border:2px solid #0f3460;border-radius:8px;"
        "font-size:16px;padding:8px 24px}"
        "QPushButton:hover{background-color:#0f3460;border-color:#e94560}"));
    btnCancel->setCursor(Qt::PointingHandCursor);
    connect(btnCancel, &QPushButton::clicked, this, &SavePage::cancelled);
    bl->addWidget(btnCancel);
    ml->addLayout(bl);

    refreshList();
}

void SavePage::refreshList()
{
    QStringList folders = m_saveSystem->saveFolderNames();
    m_table->setRowCount(0);
    m_table->setRowCount(folders.size());

    for (int i = 0; i < folders.size(); ++i)
        populateRow(i, folders[i]);
}

void SavePage::populateRow(int row, const QString &folderName)
{
    QLabel *nl = new QLabel(folderName);
    nl->setStyleSheet(kCellStyle);
    nl->setWordWrap(true);
    m_table->setCellWidget(row, 0, nl);

    SaveEntry latest = m_saveSystem->latestEntry(folderName);
    QLabel *dl = new QLabel(latest.timestamp.toString("yyyy-MM-dd HH:mm"));
    dl->setStyleSheet(kCellStyle);
    m_table->setCellWidget(row, 1, dl);

    QWidget *aw = new QWidget();
    aw->setMinimumHeight(48);
    QHBoxLayout *al = new QHBoxLayout(aw);
    al->setContentsMargins(4, 8, 4, 8);
    al->setSpacing(8);
    al->addStretch();

    QPushButton *btnOvw = new QPushButton(QStringLiteral("覆盖"));
    btnOvw->setStyleSheet(kBtnStyle);
    btnOvw->setCursor(Qt::PointingHandCursor);
    connect(btnOvw, &QPushButton::clicked, this,
            [this, folderName]() { onOverwrite(folderName); });
    al->addWidget(btnOvw);

    QPushButton *btnDel = new QPushButton(QStringLiteral("删除"));
    btnDel->setStyleSheet(kBtnStyle);
    btnDel->setCursor(Qt::PointingHandCursor);
    connect(btnDel, &QPushButton::clicked, this,
            [this, folderName]() { onDeleteFolder(folderName); });
    al->addWidget(btnDel);

    m_table->setCellWidget(row, 2, aw);
    m_table->setRowHeight(row, 64);
}

void SavePage::onNewSave()
{
    bool ok;
    QString name = QInputDialog::getText(this,
        QStringLiteral("新建存档"),
        QStringLiteral("存档名称:"),
        QLineEdit::Normal,
        QStringLiteral("存档1"), &ok);
    if (!ok || name.isEmpty()) return;
    emit saveRequested(name);
}

void SavePage::onOverwrite(const QString &folderName)
{
    auto ret = QMessageBox::question(this,
        QStringLiteral("覆盖确认"),
        QStringLiteral("确定要覆盖存档 \"%1\" 吗？").arg(folderName),
        QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes)
        emit saveRequested(folderName);
}

void SavePage::onDeleteFolder(const QString &folderName)
{
    auto ret = QMessageBox::question(this,
        QStringLiteral("删除确认"),
        QStringLiteral("确定要删除存档 \"%1\" 吗？此操作不可撤销。").arg(folderName),
        QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        m_saveSystem->deleteFolder(folderName);
        refreshList();
    }
}
