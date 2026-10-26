#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QString>
#include <QMutex>
#include <memory>

/**
 * @brief 日志系统：拦截 qDebug/qInfo/qWarning/qCritical，按天滚动写入文件
 *
 * 使用方式：
 *   Logger::instance().init("logs");
 *   qDebug() << "hello";
 */
class Logger : public QObject {
    Q_OBJECT
public:
    static Logger &instance();

    /// 初始化日志系统
    /// @param logDir 日志目录（相对于可执行文件）
    void init(const QString &logDir = QStringLiteral("logs"));

    /// 写入一条自定义日志（不走 Qt message handler）
    void write(int level, const QString &msg);

    /// 获取当前日志文件路径
    QString currentLogPath() const;

    /// 日志级别
    enum Level { Debug = 0, Info = 1, Warning = 2, Critical = 3 };

private:
    Logger() = default;
    ~Logger() override = default;
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

    static void qtMessageHandler(QtMsgType type, const QMessageLogContext &ctx, const QString &msg);
    void writeInternal(QtMsgType type, const QString &msg);
    void rotateIfNeeded();

    QString m_logDir;
    QString m_currentDate;
    QRecursiveMutex m_mutex;
};

#endif // LOGGER_H
