#ifndef __AIRPLANE_ASYNCLOGGER_H
#define __AIRPLANE_ASYNCLOGGER_H

#include <QString>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <QFile>

/**
 * @brief 异步日志单例类 (继承自 QThread)
 * 
 * 该类用于将项目运行中的各种状态信息，以极少的性能开销（非阻塞）地写入到本地文件。
 * 通过内部的一个子线程专门处理 I/O 操作（文件写入），确保主线程（如界面渲染、游戏帧更新）不会因为硬盘读写而卡顿。
 */
class AsyncLogger : public QThread 
{
    Q_OBJECT
public:
    /**
     * @brief 获取日志系统全局唯一实例（单例模式）
     * @details 主要在宏定义 LOG_INFO, LOG_WARN 等中被隐式调用。项目中任何包含该 .h 的地方均可直接调用。
     * @return 返回 AsyncLogger 的静态实例引用
     */
    static AsyncLogger& instance();

    /**
     * @brief 生产者函数：压入一条日志内容
     * @details 
     * 在 GameController.cpp 的 startGame(), checkGameOver(), logLevelStats() 等处，
     * 利用 LOG_INFO 等宏将带有中英文字符的日志内容压入后方消息队列(QQueue)。
     * 
     * @param msg 需要写入的日志文本
     */
    void log(const QString& msg);

    /**
     * @brief 停止日志子线程
     * @details 改变运行标志位并唤醒等待中的条件变量，让线程能够跳出死循环，安全退出。
     */
    void stop();

protected:
    /**
     * @brief 消费者函数：实际的文件落盘线程主体 (重写自 QThread::run)
     * @details 后台唯一一个真正对 "log/log.txt" 操作的线程入口。当系统闲置且队列无数据时休眠；
     * 有数据被 log() 投入后，被唤醒进行文件写入。
     */
    void run() override;

private:
    AsyncLogger();
    ~AsyncLogger() override;

    QQueue<QString> queue; // 日志缓冲队列 (生产者把信息塞进来，消费者线程拿走)
    QMutex mutex;          // 互斥锁，保护 queue 的多线程读写安全
    QWaitCondition cond;   // 条件变量，用于在队列为空时让线程睡眠，避免消耗 CPU (代替死循环 while(1))
    bool running;          // 子线程的存活状态控制位
};

// ---------------- 项目全局实用的便捷宏定义 ----------------
// 在工程其他文件(比如 GameController.cpp)中，调用 LOG_INFO("游戏开始"); 就会在这里通过单例写入日志
#define LOG_INFO(msg) AsyncLogger::instance().log(QStringLiteral("[INFO] ") + msg)
#define LOG_WARN(msg) AsyncLogger::instance().log(QStringLiteral("[WARN] ") + msg)
#define LOG_ERR(msg)  AsyncLogger::instance().log(QStringLiteral("[ERROR] ") + msg)

#endif // __AIRPLANE_ASYNCLOGGER_H
