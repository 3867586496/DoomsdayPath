#ifndef SMALLMAPPAGE_H
#define SMALLMAPPAGE_H

#include <QWidget>

class WorldMap;
class QTableWidget;
class QLabel;

class SmallMapPage : public QWidget
{
    Q_OBJECT

public:
    explicit SmallMapPage(WorldMap *map, QWidget *parent = nullptr);

public slots:
    void refresh();

signals:
    void returnToGame();

private:
    void setupUI();

    WorldMap *m_map;
    QTableWidget *m_table;
    QLabel *m_posLabel;
};

#endif // SMALLMAPPAGE_H
