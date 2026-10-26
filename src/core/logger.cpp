#include "logger.h"

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDebug>

// 全局 Logger 指针，供 qtMessageHandler 使用
static Logger *g_logger = nullptr;

Logger &Logger::instance()
{
    static Logger inst;
    return inst;
}

void Logger::init(const QString &logDir)
{
    QMutexLocker locker(&m_mutex);
    m_logDir = logDir;

    // 确保日志目录存在
    QDir dir(QCoreApplication::applicationDirPath());
    if (!dir.exists(logDir)) {
        dir.mkpath(logDir);
    }

    g_logger = this;

    // 安装 Qt 消息处理器
    qInstallMessageHandler(qtMessageHandler);

    // 写入一条启动日志
    writeInternal(QtInfoMsg, QStringLiteral("========== 应用启动 =========="));
}

QString Logger::currentLogPath() const
{
    QDir dir(QCoreApplication::applicationDirPath());
    return dir.filePath(m_logDir + QStringLiteral("/app_") + m_currentDate + QStringLiteral(".log"));
}

void Logger::write(int level, const QString &msg)
{
    QtMsgType type = QtInfoMsg;
    switch (level) {
    case Debug:    type = QtDebugMsg; break;
    case Info:     type = QtInfoMsg; break;
    case Warning:  type = QtWarningMsg; break;
    case Critical: type = QtCriticalMsg; break;
    }
    writeInternal(type, msg);
}

void Logger::rotateIfNeeded()
{
    QString today = QDate::currentDate().toString(QStringLiteral("yyyy-MM-dd"));
    if (today != m_currentDate) {
        m_currentDate = today;
    }
}

void Logger::writeInternal(QtMsgType type, const QString &msg)
{
    QMutexLocker locker(&m_mutex);
    rotateIfNeeded();

    QString levelStr;
    switch (type) {
    case QtDebugMsg:    levelStr = QStringLiteral("DEBUG"); break;
    case QtInfoMsg:     levelStr = QStringLiteral("INFO "); break;
    case QtWarningMsg:  levelStr = QStringLiteral("WARN "); break;
    case QtCriticalMsg: levelStr = QStringLiteral("ERROR"); break;
    case QtFatalMsg:    levelStr = QStringLiteral("FATAL"); break;
    }

    QString timestamp = QDateTime::currentDateTime()
                            .toString(QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz"));
    QString line = QStringLiteral("[%1] [%2] %3\n").arg(timestamp, levelStr, msg);

    QString path = currentLogPath();

    QFile file(path);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << line;
        stream.flush();
    }
    // 同时也输出到控制台（release 下可注释掉）
    fprintf(stderr, "%s", line.toUtf8().constData());
}

void Logger::qtMessageHandler(QtMsgType type, const QMessageLogContext &ctx, const QString &msg)
{
    Q_UNUSED(ctx);
    if (g_logger) {
        g_logger->writeInternal(type, msg);
    }
}
