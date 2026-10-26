#ifndef PROTOCOLBASE_H
#define PROTOCOLBASE_H

#include "iprotocolworker.h"
#include <QTimer>

/**
 * @brief 协议 Worker 公共基类
 *
 * 提供常用成员（连接状态、轮询定时器、统一的状态接口），
 * 具体协议只需要覆写 doOpen/doClose/doPoll/parseFrame 等纯虚函数。
 */
class ProtocolBase : public IProtocolWorker {
    Q_OBJECT
public:
    explicit ProtocolBase(QObject *parent = nullptr);
    ~ProtocolBase() override;

    bool isConnected() const { return m_connected; }
    int  pollIntervalMs() const { return m_pollIntervalMs; }

public slots:
    /// 设置轮询周期（毫秒，0 表示不轮询）
    void setPollInterval(int ms);

    /// 启动/停止自动轮询
    void startPolling();
    void stopPolling();

protected slots:
    /// 轮询定时器触发
    virtual void onPollTimer();

protected:
    /// 由子类实现实际打开连接
    virtual bool doOpen() = 0;

    /// 由子类实现实际关闭连接
    virtual bool doClose() = 0;

    /// 由子类实现实际轮询动作
    virtual void doPoll() {}

    /// 统一设置连接状态并发射信号
    void setConnected(bool c);

    bool m_connected = false;
    int  m_pollIntervalMs = 1000;
    QTimer *m_pollTimer = nullptr;
};

#endif // PROTOCOLBASE_H
