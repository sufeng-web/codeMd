#ifndef __GAMEVIEW_H
#define __GAMEVIEW_H

#include <QWidget>
#include <QLabel>
#include <QPaintEvent>
#include <QKeyEvent>
#include <QPushButton>
#include "../include/IObserver.h"
#include "../Model/GameData.h"
#include "../Model/Fruit.h"

// 前置声明，打断相互包含的循环编译
class GameController;

/**
 * @brief 游戏主视图 / 界面 (View 层) + 观察者 (IObserver)
 * 
 * 职责 1：负责在屏幕上把背景、苹果、分数面板、控制按钮【画】出来 (paintEvent)。
 * 职责 2：负责【监听用户所有的实体操作】。只要按下键盘，抓住不放直接丢给 Controller (keyPressEvent 发出 signal)。
 * 职责 3：贯彻【观察者模式(Observer)】。实现了 `onUpdate()` 方法，当数据层 `GameData` 里有分数或者生命值更新时，
 *          它会强制调取这个 onUpdate 重新刷新标签。
 */
class GameView : public QWidget, public IObserver {
     Q_OBJECT
public:
    /**
     * @brief 构造函数：初始化并绘制整个游戏的骨架 UI
     * @details 会把自己作为观察者 `data->attach(this);` 挂在数据模型上，初始化所有的控制按钮和右侧面板的分数。
     */
    GameView(GameData* data, QWidget* parent = nullptr);
    
    /**
     * @brief 注入大脑控制中枢
     * @details 在主菜单 MainMenu 打开苹果游戏时，把组装好用来做业务判断的 GameController 塞进来。
     */
    void setController(GameController* c) { controller = c; }

    /**
     * @brief IObserver 观察者的实现
     * @details 当模型 `GameData::notifyObservers()` 被触发时回调该函数。
     * 在这里把数值面板上的 "分：XXXX" 和右侧的漏接数全部进行 setText 动态更新刷新。
     */
    void onUpdate() override;  
    
    // 渲染下落单个苹果的动画、字母贴图的方法（在 paintEvent 里被遍历调用）
    void renderFruit(GameController* controller, Fruit* fruit);
    void showGameOver();
    void showPauseMenu();
    
    /**
     * @brief 一键根据贴图坐标建立底部菜单按钮的模板辅助函数
     */
    template<typename Func>
    void createMenuBtn(QPushButton*& btn, const QString& img1, const QString& img2, int x, int y, Func&& signal);

protected:
    /**
     * @brief Qt 的核心绘图事件 (画面渲染心跳)
     * @details 
     * 由外部（或者是 GameController 的 updateGame 定时器）利用 view->update() 进行高频呼叫。
     * 每次呼叫在这里会拿取最新的模型数据，从头开始贴草地背景、贴所有的水果位置。
     */
    void paintEvent(QPaintEvent* event) override;
    
    /**
     * @brief Qt 的键盘监听事件
     * @details 游戏玩家拍下 A 或者 B 都会在这里截获，转换成信号 keyPressed 给 Controller 拦截结算。
     */
    void keyPressEvent(QKeyEvent* event) override;
    
signals:
    // ===============  UI 向控制室发出的指令动作 (被 Controller 捕获)  ===============
    void keyPressed(int key);  // 发送键盘A-Z的 Ascii 给Controller 的 handleKeyPress
    void sigStart();           // 点了下方的【开始】按钮
    void sigPause();           // 点了下方【暂停】按钮
    void sigEnd();             // 点了下方【结束】按钮
    void sigSetup();           // 点了下方齿轮【设置】按钮 (弹出修改生命难度弹窗)
    void sigExit();            // 彻底点击【退出】界面
    
private:
    GameController* controller = nullptr;
    GameData* gameData; // 绑定的游戏数据引用源文件
    
    QLabel* scoreLabel; // 左置顶：分数展示标签
    QLabel* levelLabel; // 难度等级标签
    QLabel* infoLabel;  // 漏接/苹果篮子概括标签
    
    QPushButton* btnExit;
    QPushButton* btnSetup;
    QPushButton* btnStart;
    QPushButton* btnPause;
    QPushButton* btnEnd;
};
#endif