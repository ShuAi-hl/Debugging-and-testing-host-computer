#include "protocolbase.h"
#include "../core/logger.h"

ProtocolBase::ProtocolBase(QObject *parent)
    : IProtocolWorker(parent)
{
    m_pollTimer = new QTimer(this);
    m_pollTimer->setSingleShot(false);
    connect(m_pollTimer, &QTimer::timeout, this, &ProtocolBase::onPollTimer);
}

ProtocolBase::~ProtocolBase()
{
    if (m_pollTimer) m_pollTimer->stop();
}

void ProtocolBase::setPollInterval(int ms)
{
    m_pollIntervalMs = ms;
    if (m_pollTimer->isActive()) {
        m_pollTimer->start(ms);
    }
}

void ProtocolBase::startPolling()
{
    if (m_pollIntervalMs <= 0) m_pollIntervalMs = 1000;
    m_pollTimer->start(m_pollIntervalMs);
    qInfo() << protocolName() << "轮询启动, 周期 =" << m_pollIntervalMs << "ms";
}

void ProtocolBase::stopPolling()
{
    m_pollTimer->stop();
    qInfo() << protocolName() << "轮询停止";
}

void ProtocolBase::onPollTimer()
{
    if (!m_connected) return;
    doPoll();
}

void ProtocolBase::setConnected(bool c)
{
    if (m_connected != c) {
        m_connected = c;
        emit connectionStateChanged(c);
        qInfo() << protocolName() << "连接状态:" << (c ? "已连接" : "已断开");
    }
}
