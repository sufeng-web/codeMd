#pragma once
#include <QWidget>
#include <QTimer>
#include <QKeyEvent>
#include <QPainter>
#include <vector>
#include<QPushButton>
//苹果的横纵坐标 与 字母信息
struct Apple
{
    int x;
    float y;
    char letter;
};




/*
//State:游戏运行状态 空闲-进行中-暂停-结束
//apples:屏幕当前显示的所有苹果   容器
//level ：当前关卡等级。在 updateGame 中决定苹果的下落速度（speed = 1.0f + level * 0.5f;），关卡越高下落越快。
// successCount ：当前关卡已成功击中的苹果数量。当这个数值达到通关目标时，会触发关卡升级。
// failCount ：漏掉的苹果（掉到了屏幕底部的红线以下）的数量统计。后续在 paintEvent 中会把它绘制到屏幕上向玩家展示，通常也会用来判断游戏失败条件（Game Over）。
// totalPresses ;：玩家总共按键的次数。可以用来计算玩家的“按键准确率”（命中数 / 总按键数）。
// targetSuccess ：当前关卡的通关目标。玩家打中 10 个苹果即可进入下一关。
// maxApples ;：屏幕上同屏允许存在的最大苹果数量。在 spawnApple 函数首行拦截，防止满屏幕都是苹果导致玩家根本看不过来
*/

class AppleGame : public QWidget
{
    Q_OBJECT
public:
    AppleGame(QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
private slots:
    void updateGame();
    void onbtnExitClick();
    void onbtnEndClick();
    void onbtnSetClick();
    void onbtnStopClick();
    void onbtnStartClick();
private:
    void spawnApple();
    void cnnt();
    void setApple(int level, int successCount, int failCount, int totalPresses, int targetSuccess, int maxApples);
    void uiAppleDraw();
    enum State 
    { 
        Idle,
        Playing,
        Paused,
        End 
    } state;
    QTimer *timer;
    
    std::vector<Apple> apples;
    QPushButton* btnExit;
    QPushButton* btnEnd;
    QPushButton* btnStop;
    QPushButton* btnSet;
    QPushButton* btnStart;

    int m_level = 1;
    int m_successCount = 0;
    int m_failCount = 0;
    int m_totalPresses = 0;
    int m_targetSuccess = 10;
    int m_maxApples = 3;
};

