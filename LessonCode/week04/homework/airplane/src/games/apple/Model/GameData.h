#pragma once

#include <vector>
#include "../include/IObserver.h"

class GameData {
public:
    GameData();

    // 分数管理
    int getScore() const;
    void addScore(int value);
    void resetScore();

    // 难度等级
    int getLevel() const;
    void setLevel(int level);

    // 未接到的苹果管理
    int getMissed() const;
    void addMissed();
    void resetMissed();
    int getMaxMisses() const;
    void setMaxMisses(int maxMisses);

    // 生命值管理（保留以兼容，但主要使用 Missed）
    int getLives() const;
    void setLives(int lives);
    void loseLife();
    void addLife();

    // 篮子苹果管理
    int getBasketApples() const;
    void addBasketApple();
    void clearBasketApples();

    // 游戏状态
    bool isGameOver() const;
    bool isPaused() const;
    void setPaused(bool paused);
    void setGameOver(bool gameOver);

    // 观察者模式：通知UI更新
    void attach(IObserver* observer);
    void detach(IObserver* observer);

private:
    void notifyObservers();

    int m_score;
    int m_lives;
    int m_level;
    int m_missed;
    int m_maxMisses;
    int m_basketApples;
    bool m_isPaused;
    bool m_isGameOver;
    std::vector<IObserver*> m_observers;
};
