#include "Tracker.h"
#include <QDateTime>

// ==========================================
// Tracker.cpp 
// 项目中的全局游戏数据埋点(Event Tracking)单例。
// 把玩家通关(Level_Stats)、吃苹果(Apple_Hit)等核心业务数据丢到一个特定文件(tracker.log)以备数据分析。
// ==========================================

Tracker::Tracker() : running(true) { start(); }

Tracker::~Tracker() {
    stop();
    wait();
}

Tracker& Tracker::instance() 
{
    static Tracker instance;
    return instance;
}

/**
 * @brief 接收一条事件信息，加上特定的 “时间 + 标签 + 数据体” 的格式投递入后台队列。
 * @param eventName 例如进入关卡时的 "Game_Start"，以及 GameController 按下按钮击杀下落的苹果 "Apple_Hit"
 * @param eventData 解析的中文或英文内容数据，如 "Score: 100"，或 "第 1 关 使用按键 ...失败次数..."
 * @details 
 * 在 GameController.cpp 的 handleKeyPress() 内每次苹果对象(*it)->getLetter() 成功消除时，
 * 都会调用这行代码进行单次准确追踪。配合队列 QWaitCondition 的唤醒实现无阻塞游戏反馈。
 */
void Tracker::trackEvent(const QString& eventName, const QString& eventData) 
{
    QMutexLocker locker(&mutex);
    queue.enqueue(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") + " | Event: " + eventName + " | Data: " + eventData);
    cond.wakeOne();
}

void Tracker::stop() 
{
    QMutexLocker locker(&mutex);
    running = false;
    cond.wakeOne();
}

/**
 * @brief 后台文件 IO 消费者线程
 * @details 这是和 AsyncLogger 共生的同等底层重写；它创建的是根目录运行程序边上的 "tracker.log"。
 */
void Tracker::run() {
    QFile file("tracker.log");
    // 不开启纯文本转化（防乱码），完全使用二进制的 UTF-8 注入解决跨语言（特别是游戏中文结算）丢失的情况。
    if (!file.open(QIODevice::Append | QIODevice::Text)) return;

    while (true) {
        QString msg;
        {
            QMutexLocker locker(&mutex);
            // 空拿休眠：没按过键、没漏接苹果就不会有 tracker() 调用，线程在这里安静。
            while (queue.isEmpty() && running) {
                cond.wait(&mutex); 
            }
            if (!running && queue.isEmpty()) {
                break;
            }
            if (!queue.isEmpty()) {
                msg = queue.dequeue();
            }
        }
        if (!msg.isEmpty()) {
            file.write((msg + "\n").toUtf8());  // 强转底层 UTF-8 内存字符编码。
            file.flush(); // 即刻刷新缓存，如果玩家突然强点退出菜单，保证所有正确率等埋点结算强制落地！
        }
    }
}
