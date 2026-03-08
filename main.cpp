#include <QApplication>
#include <QStyleFactory>

#include "src/mainwindow.hpp"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    MainWindow window;
    window.show();
    return app.exec();
}