#ifndef LOADGAMEPAGE_H
#define LOADGAMEPAGE_H

#include <QWidget>

class QTableWidget;
class QPushButton;
class SaveSystem;

class LoadGamePage : public QWidget
{
    Q_OBJECT

public:
    explicit LoadGamePage(SaveSystem *saveSystem, QWidget *parent = nullptr);

    void refreshList();

signals:
    void backClicked();
    void loadRequested(const QString &folderName, int entryIndex);

private slots:
    void onDeleteFolder(const QString &folderName);
    void onLoadFolder(const QString &folderName);
    void onShowDetails(const QString &folderName);

private:
    void setupUI();
    void populateFolderRow(int row, const QString &folderName);

    SaveSystem *m_saveSystem;
    QTableWidget *m_table;
    QPushButton *m_btnBack;
};

#endif // LOADGAMEPAGE_H
