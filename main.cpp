#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication a(argc, argv);

//    w.setWindowTitle(QObject::tr("iLighting config tool"));
    QTranslator translator;
//    translator.load("C:/Users/Administrator.LA22NR22AFOORAV/Desktop/iLighting-config/iLighting-conifg_zh_CN.qm");
    translator.load("iLighting-conifg_zh_CN.qm");
    a.installTranslator(&translator);

    MainWindow w;
    w.show();
    return a.exec();
}
