#include "mainwindow.h"
#include "core/logger.h"

#include <QApplication>
#include <QScreen>
#include <QGuiApplication>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 日志系统：拦截 qDebug/qInfo 等，按天滚动写入可执行文件所在的 logs/
    Logger::instance().init(QStringLiteral("logs"));

    QApplication::setApplicationName(QStringLiteral("通用工业设备调试上位机"));

    MainWindow w;

    // 居中显示
    if (auto *screen = QGuiApplication::primaryScreen()) {
        QRect avail = screen->availableGeometry();
        w.move(avail.center().x() - w.width()/2, avail.center().y() - w.height()/2);
    }

    w.show();
    return a.exec();
}
