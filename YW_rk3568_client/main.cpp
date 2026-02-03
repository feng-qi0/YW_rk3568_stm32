#include "rk3568controlpanel.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RK3568ControlPanel w;
    w.show();
    return a.exec();
}
