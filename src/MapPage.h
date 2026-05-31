#ifndef MAPPAGE_H
#define MAPPAGE_H

#include <QWidget>

class WorldMap;
class QGridLayout;
class QKeyEvent;
class QLabel;
class QPushButton;

class MapPage : public QWidget
{
    Q_OBJECT

public:
    explicit MapPage(WorldMap *map, QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void showEvent(QShowEvent *event) override;

signals:
    void returnToGame();
    void tileEntered(int x, int y, int cost);

private slots:
    void refreshGrid();
    void tryMove(int dx, int dy);

private:
    void setupUI();
    void updateTileButton(int gridRow, int gridCol, int mapX, int mapY);
    QWidget *createTileWidget(int mapX, int mapY);

    WorldMap *m_map;
    QGridLayout *m_gridLayout;
    QLabel *m_infoLabel;

    static constexpr int ViewRadius = 4;
    static constexpr int CellSize = 64;
};

#endif // MAPPAGE_H
