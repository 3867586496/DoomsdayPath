#ifndef SAVESYSTEM_H
#define SAVESYSTEM_H

#include "PlayerStats.h"
#include "Item.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <vector>

struct SaveEntry
{
    QString name;
    int day = 1;
    int hour = 6;
    int minute = 0;
    double hp = 100;
    double hunger = 100;
    double thirst = 100;
    double sanity = 100;
    double rest = 100;
    double difficultyFactor = 1.0;
    std::vector<Item> items;
    QDateTime timestamp;
    bool isAuto = false;

    QJsonObject toJson() const;
    static SaveEntry fromJson(const QJsonObject &obj);
};

class SaveSystem : public QObject
{
    Q_OBJECT

public:
    static constexpr int DefaultMaxAutoSaves = 5;

    explicit SaveSystem(QObject *parent = nullptr);

    int maxAutoSaves() const { return m_maxAutoSaves; }
    void setMaxAutoSaves(int n) { m_maxAutoSaves = qMax(1, n); }

    // List all save folders (big folders containing individual saves)
    QStringList saveFolderNames() const;

    // Get save entries in a folder
    std::vector<SaveEntry> entriesInFolder(const QString &folderName) const;

    // Save a new entry
    bool saveEntry(const QString &folderName, const SaveEntry &entry);

    // Delete a save folder
    bool deleteFolder(const QString &folderName);

    // Delete an individual entry in a folder
    bool deleteEntry(const QString &folderName, int index);

    // Get the latest entry in a folder
    SaveEntry latestEntry(const QString &folderName) const;

    // Base directory for saves
    static QString saveDir();

signals:
    void savesChanged();

private:
    QString folderPath(const QString &folderName) const;
    void ensureDir(const QString &path) const;
    int m_maxAutoSaves = DefaultMaxAutoSaves;
};

#endif // SAVESYSTEM_H
