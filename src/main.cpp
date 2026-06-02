#include <QApplication>
#include "MainWindow.h"
#include "style.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Global base stylesheet — every widget inherits this
    app.setStyleSheet(Style::kAppStyle
        .arg(Style::kDarkBg, Style::kText,
             Style::kPanelBg, Style::kBorder, Style::kAccent));

    MainWindow window;
    window.show();
    return app.exec();
}
