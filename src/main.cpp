#include <QApplication>
#include <QTimer>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    QTimer::singleShot(500, &window, &QMainWindow::close);
    return app.exec();
}
