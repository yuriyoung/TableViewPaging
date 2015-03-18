#include "WindowEx.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WindowEx w;
    w.show();

    return a.exec();
}
