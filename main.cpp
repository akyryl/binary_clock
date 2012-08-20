#include <QApplication>
#include <QPushButton>

#include "clock.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Clock clock;
    clock.show();
    return app.exec();
}
