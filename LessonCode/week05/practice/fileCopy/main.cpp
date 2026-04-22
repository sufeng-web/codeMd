#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    
    w.setWindowTitle("极速批量文件复制工具");

    w.show();

    return a.exec();
}