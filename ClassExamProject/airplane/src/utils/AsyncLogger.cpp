#include "AsyncLogger.h"
#include <QDateTime>
#include <QCoreApplication>
#include <QDir>
#include <QDebug>

// ==========================================
// AsyncLogger.cpp 
// 项目中的全局异步日志文件写入器。
// 主要解决 QThread 消费者/生产者模式，将日志落到可执行文件的 `/log/log.txt` 中。
// ==========================================

/**
 * @brief 构造函数：单例被首次创建时拉起后台 QThread，开始循环监听
 * @details 随 instance() 的调用而分配，全局只创建一次，并随着程序启动立即 start()
 */
AsyncLogger::AsyncLogger() : running(true) 
{
    start(); // 启动由 QThread 继承来的 run() 线程函数，使消费者准备就绪
}

/**
 * @brief 析构函数：保证全局清理退出无异常
 * @details 项目主应用程序退出(如调用 main.cpp的 a.exec() 结束)时，会由编译器清理该静态实例并进入析构中，强制让异步线程 stop() 且 wait()，等待所有内存中的日志被写入后再清理。
 */
AsyncLogger::~AsyncLogger() 
{
    stop();  // 修改 running 并发出信号
    wait();  // 同步主线程等待这个后台写日志的线程跑完 `run` 收尾退出，避免程序瞬间“撕裂/强杀”导致残缺
}

/**
 * @brief 暴露实例给项目内的各文件取用
 * @details C++ 单例特性（静态局部变量）：
 * 当 GameController 执行如 LOG_INFO("Game starting..."); 的宏时，首次进入。
 * 返回该类的引用。
 */
AsyncLogger& AsyncLogger::instance() 
{
    static AsyncLogger instance;
    return instance;
}

/**
 * @brief 向日志队列中投币（生产者），由宏调用如 `LOG_INFO(...)` 触发。
 * @param msg 日期拼接好的字符串消息，比如： "[INFO] 第 1 关 用了什么按键: [A B]，正确率: 100.0%, 失败次数: 0"
 * @details
 * 加锁保证如果两个不同的地方同时 `log(...)`，数据不会错乱。完成后唤醒由于没票而堵在 cond.wait() 的后台线程。
 */
void AsyncLogger::log(const QString& msg) 
{
    QMutexLocker locker(&mutex);
    // 将获得的中英文日志，在前面拼上一条系统当前具体精确时间前置，进队
    queue.enqueue(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") + " | " + msg);
    // 队列里进东西了！唤醒消费者（也就是下面run里面还在休眠的 cond.wait）。
    cond.wakeOne();
}

/**
 * @brief 通过外部改变标识位使得后台死循环能够停下，随后再用 wait 清理。
 */
void AsyncLogger::stop() 
{
    QMutexLocker locker(&mutex);
    running = false;
    cond.wakeOne(); // 唤醒还在休眠的 wait，让他去检查 running=false，然后 break 出口。
}

/**
 * @brief 后台日志常驻线程核心逻辑 (消费者)
 * @details
 * 它不会阻塞 `QApplication` 主界面，也不会卡住通过定时器在跑的游戏循环（比如下落和结算函数）。
 * 不论 `GameController` 或者 `MainMenu` 怎么卡，后台的 `run()` 只看 `queue` 是否为空；
 * 如果空则休眠（cond.wait），如果不空且没停止则出队落盘（QFile.write）。
 */
void AsyncLogger::run()
{
    // 获取当前 .exe 所运行的根级绝对真实路径（例如 D:/xxx/out/build/debug/airplane/）
    // 强制和程序代码锁死在同级，不受系统 IDE 与终端启动影响，确保玩家找得到日志。
    QString appDir = QCoreApplication::applicationDirPath();
    
    // 构造日志目录 airplane/log/
    QDir dir(appDir);
    if (!dir.exists("log"))
    {
        dir.mkdir("log"); // 专门创建放置 log.txt 等监控文件的 log 专属文件夹。
    }

    QString logFilePath = QDir::cleanPath(appDir + "/log/log.txt");
    QFile file(logFilePath);

    // 以Append（尾部追加）形式，把 file 句柄直接贴在 txt 上，不擦除之前游戏产生的历史。
    if (!file.open(QIODevice::Append | QIODevice::Text | QIODevice::WriteOnly))
    {
        qWarning() << "Failed to open log file:" << file.errorString() << "at path:" << logFilePath;
        return;
    }

    // 后台纯死循环
    while (true)
    {
        QString msg;
        {
            // 多线程第一步，先拿到管理这批内存的锁，锁住队列进行判定
            QMutexLocker locker(&mutex);

            // 当队列没东西了，并且咱们还没退出程序，就进入休眠，别像 while(1) 空转让 CPU 发热。
            while (queue.isEmpty() && running)
            {
                cond.wait(&mutex); // 被唤醒的条件： 1. 外面通过调用 stop() 唤醒  2. 队列外面有新的数据发过来 `log()` 了唤醒
            }

            // 1. 如果是被 stop() 通知并且队列空了（清空了所有玩家打字数据后，游戏终于退出） —— break 结束！
            if (queue.isEmpty())
            {
                break;
            }

            // 2. 排除因为 stop 而被唤醒的情况。能运行到这说明是：【队列有日志任务了！】
            msg = queue.dequeue();
        }

        // 把日志推到真实的磁盘上去
        if (!msg.isEmpty()) 
        {
            // 这是修复 bug 的最重要一句代码！（.toUtf8()）
            // 如果不写强制 UTF-8，默认通过 QTextStream 去强转宽字符QString时，Windows 中文（如“第1关 用了啥按键”）
            // 会把流的系统自带本地编码映射搞崩溃，之后会拒绝一切内容的输入，导致日志“只能写入英文 Game Start，进不去中文”。
            file.write((msg + "\n").toUtf8());
            file.flush();  // 逼迫 QFile 立刻将文件 IO 的临时缓冲区强写到硬盘，防止游戏通过 [X] 被瞬间强杀掉出 bug 日志为空！
        }
    }
    // 线程安全退出前（运行了 stop 然后跑到 break 外后），正式放开并且关闭这个 log.txt 被锁定的文件句柄。
    file.close();
}
