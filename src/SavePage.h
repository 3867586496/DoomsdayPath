#ifndef SAVEPAGE_H
#define SAVEPAGE_H

#include <QWidget>

class QTableWidget;
class QPushButton;
class SaveSystem;

class SavePage : public QWidget
{
    Q_OBJECT

public:
    explicit SavePage(SaveSystem *saveSystem, QWidget *parent = nullptr);

    void refreshList();

signals:
    void cancelled();
    void saveRequested(const QString &folderName);

private slots:
    void onNewSave();
    void onOverwrite(const QString &folderName);
    void onDeleteFolder(const QString &folderName);

private:
    void setupUI();
    void populateRow(int row, const QString &folderName);

    SaveSystem *m_saveSystem;
    QTableWidget *m_table;
};

#endif // SAVEPAGE_H
