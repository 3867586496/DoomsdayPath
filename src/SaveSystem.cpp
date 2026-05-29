#include "SaveSystem.h"

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

// --- SaveEntry ---
QJsonObject SaveEntry::toJson() const
{
    QJsonObject obj;
    obj["name"] = name;
    obj["day"] = day;
    obj["hour"] = hour;
    obj["minute"] = minute;
    obj["hp"] = hp;
    obj["hunger"] = hunger;
    obj["thirst"] = thirst;
    obj["sanity"] = sanity;
    obj["difficultyFactor"] = difficultyFactor;
    obj["isAuto"] = isAuto;
    obj["timestamp"] = timestamp.toString(Qt::ISODate);

    QJsonArray itemArray;
    for (const auto &item : items) {
        QJsonObject i;
        i["name"] = item.name();
        i["description"] = item.description();
        i["weight"] = item.weight();
        i["edible"] = item.isEdible();
        QJsonArray effArray;
        for (const auto &e : item.effects()) {
            QJsonObject eo;
            eo["target"] = static_cast<int>(e.target);
            eo["amount"] = e.amount;
            effArray.append(eo);
        }
        i["effects"] = effArray;
        itemArray.append(i);
    }
    obj["items"] = itemArray;
    return obj;
}

SaveEntry SaveEntry::fromJson(const QJsonObject &obj)
{
    SaveEntry e;
    e.name = obj["name"].toString();
    e.day = obj["day"].toInt(1);
    e.hour = obj["hour"].toInt(6);
    e.minute = obj["minute"].toInt(0);
    e.hp = obj["hp"].toDouble(100);
    e.hunger = obj["hunger"].toDouble(100);
    e.thirst = obj["thirst"].toDouble(100);
    e.sanity = obj["sanity"].toDouble(100);
    e.difficultyFactor = obj["difficultyFactor"].toDouble(1.0);
    e.isAuto = obj["isAuto"].toBool(false);
    e.timestamp = QDateTime::fromString(obj["timestamp"].toString(), Qt::ISODate);

    QJsonArray itemArray = obj["items"].toArray();
    for (const auto &iv : itemArray) {
        QJsonObject i = iv.toObject();
        std::vector<StatChange> effects;
        QJsonArray effArray = i["effects"].toArray();
        for (const auto &ev : effArray) {
            QJsonObject eo = ev.toObject();
            effects.push_back({
                static_cast<StatChange::Target>(eo["target"].toInt()),
                eo["amount"].toDouble()
            });
        }
        e.items.push_back(Item(
            i["name"].toString(),
            i["description"].toString(),
            i["weight"].toDouble(),
            effects,
            i["edible"].toBool()
        ));
    }
    return e;
}

// --- SaveSystem ---
SaveSystem::SaveSystem(QObject *parent) : QObject(parent)
{
    ensureDir(saveDir());
}

QString SaveSystem::saveDir()
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
           + "/saves";
}

QString SaveSystem::folderPath(const QString &folderName) const
{
    return saveDir() + "/" + folderName;
}

void SaveSystem::ensureDir(const QString &path) const
{
    QDir dir(path);
    if (!dir.exists()) dir.mkpath(".");
}

QStringList SaveSystem::saveFolderNames() const
{
    QDir dir(saveDir());
    return dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Time);
}

std::vector<SaveEntry> SaveSystem::entriesInFolder(const QString &folderName) const
{
    std::vector<SaveEntry> entries;
    QDir dir(folderPath(folderName));
    QStringList files = dir.entryList({"*.json"}, QDir::Files, QDir::Name | QDir::Reversed);
    for (const auto &file : files) {
        QFile f(dir.filePath(file));
        if (!f.open(QIODevice::ReadOnly)) continue;
        QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
        f.close();
        if (doc.isObject())
            entries.push_back(SaveEntry::fromJson(doc.object()));
    }
    return entries;
}

bool SaveSystem::saveEntry(const QString &folderName, const SaveEntry &entry)
{
    ensureDir(folderPath(folderName));
    QString fileName = entry.isAuto
        ? QStringLiteral("auto_%1_%2.json")
              .arg(entry.day, 3, 10, QLatin1Char('0'))
              .arg(entry.timestamp.toString("yyyyMMdd_HHmmss"))
        : QStringLiteral("manual_%1.json")
              .arg(entry.timestamp.toString("yyyyMMdd_HHmmss"));

    // Prune auto saves if exceeding limit
    if (entry.isAuto) {
        auto existing = entriesInFolder(folderName);
        int autoCount = 0;
        for (const auto &e : existing) {
            if (e.isAuto) autoCount++;
        }
        if (autoCount >= MaxAutoSaves) {
            // Delete oldest auto save
            QDir dir(folderPath(folderName));
            QStringList autoFiles = dir.entryList({"auto_*.json"}, QDir::Files, QDir::Name);
            if (!autoFiles.isEmpty())
                QFile::remove(dir.filePath(autoFiles.first()));
        }
    }

    QFile f(folderPath(folderName) + "/" + fileName);
    if (!f.open(QIODevice::WriteOnly)) return false;
    f.write(QJsonDocument(entry.toJson()).toJson());
    f.close();
    emit savesChanged();
    return true;
}

bool SaveSystem::deleteFolder(const QString &folderName)
{
    QDir dir(folderPath(folderName));
    if (dir.exists()) {
        dir.removeRecursively();
        emit savesChanged();
        return true;
    }
    return false;
}

bool SaveSystem::deleteEntry(const QString &folderName, int index)
{
    QDir dir(folderPath(folderName));
    QStringList files = dir.entryList({"*.json"}, QDir::Files, QDir::Name | QDir::Reversed);
    if (index < 0 || index >= files.size()) return false;
    bool ok = QFile::remove(dir.filePath(files[index]));
    if (ok) emit savesChanged();
    return ok;
}

SaveEntry SaveSystem::latestEntry(const QString &folderName) const
{
    auto entries = entriesInFolder(folderName);
    if (entries.empty()) return {};
    // Return latest by timestamp
    SaveEntry latest = entries[0];
    for (const auto &e : entries) {
        if (e.timestamp > latest.timestamp)
            latest = e;
    }
    return latest;
}
