#pragma once
#ifndef __GAMEDATA_H_
#define __GAMEDATA_H_
#include <vector>
#include "../include/IObserver.h"

/**
 * @brief 游戏核心数据模型 (Model层)
 * 
 * 存放了游戏里所有的数值：得分、关卡难度、漏掉的苹果数、暂停与结束状态等。
 * 运用了【观察者模式(Observer)】。由于 UI(界面) 无法主动知道数据什么时候变了，
 * GameData 在每次数据发生修改时（如 addScore 或 addMissed）会主动调用 notifyObservers() 
 * 来提醒订阅了此事件的 GameView/其他视图进行数据的强制重绘。
 */
class GameData {
public:
    GameData();

    // ================= 分数管理 =================
    int getScore() const;
    /**
     * @brief 增加分数，并在内部调用 notifyObservers() 提醒界面的分数文本刷新
     * @details 在 GameController 当击中字母时，按苹果自身预设的基础分加入
     * @param value 增加的分数
     */
    void addScore(int value);
    void resetScore();

    // ================= 难度等级 =================
    int getLevel() const;
    /**
     * @brief 修改当前所在的关卡等级
     * @details 由 GameController 判定满一百分则提升，此时会影响之后苹果下落基准速度。并且要求视图重绘等级标签。
     */
    void setLevel(int level);

    // ============== 未接到(失败)的苹果管理 =============
    int getMissed() const;
    /**
     * @brief 增加一次漏接记录，触发 Observer 更新界面右侧失败数。
     * @details 在 GameController 的 checkCollisions() 里当苹果越过底线坠毁时调用。
     */
    void addMissed();
    void resetMissed();
    
    int getMaxMisses() const; // 读取系统当前允许的生命漏接上限
    void setMaxMisses(int maxMisses);

    // ================= 历史保留：生命值管理（建议迁移至Missed） =================
    int getLives() const;
    void setLives(int lives);
    void loseLife();
    void addLife();

    // ================= 篮子苹果管理 (画面收集元素) =================
    int getBasketApples() const;
    void addBasketApple(); // 击中一枚，向界面展示的虚拟篮子投入一个计数
    void clearBasketApples();

    // ================= 游戏运行主状态 =================
    bool isGameOver() const;
    bool isPaused() const;
    
    // 设置暂停：比如弹出过关框，或者玩家按暂停时改变状态流
    void setPaused(bool paused);
    // 决定这局是否由于 Missed >= MaxMisses 或通关结束
    void setGameOver(bool gameOver);

    // ================= 观察者模式：推送 UI 更新 =================
    /**
     * @brief 注册视图监听者
     * @details GameView 初始化时会把自己 attach 挂载进这个模型。
     */
    void attach(IObserver* observer);
    /**
     * @brief 注销视图监听者
     */
    void detach(IObserver* observer);

private:
    /**
     * @brief 一旦核心数值发生变化，遍历所有已注册的视图（如 GameView）请求其更新
     */
    void notifyObservers();

    int m_score;
    int m_lives;
    int m_level;
    int m_missed;
    int m_maxMisses;
    int m_basketApples;
    bool m_isPaused;
    bool m_isGameOver;
    std::vector<IObserver*> m_observers; // 在此维护所有关心数据的视图对象 (GameView)
};
#endif