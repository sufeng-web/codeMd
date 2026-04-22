#ifndef TRACKER_H
#define TRACKER_H

#include <QString>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <QFile>

/**
 * @brief 数据埋点(追踪)单例类
 * 
 * 类似 AsyncLogger，Tracker 主要用来统计【游戏运营与测试人员】分析需要的数据：
 * 什么时候过了什么关，分数的节点是多少。玩家操作的正确率有多少？
 * 本质也是多线程队列机制，可以脱离游戏主渲染逻辑（GameController 的 updateGame 循环）后台独立运行。
 */
class Tracker : public QThread 
{
    Q_OBJECT
public:
    static Tracker& instance();

    /**
     * @brief 上报与记录核心方法
     * @param eventName 事件类别 (如同 GameController 里的 "Game_Start", "Apple_Hit", "Level_Stats")
     * @param eventData 事件附带的具体数据解析或中文长句
     */
    void trackEvent(const QString& eventName, const QString& eventData);

    void stop();

protected:
    void run() override;

private:
    Tracker();
    ~Tracker() override;

    QQueue<QString> queue;
    QMutex mutex;
    QWaitCondition cond;
    bool running;
};

#endif // TRACKER_H
