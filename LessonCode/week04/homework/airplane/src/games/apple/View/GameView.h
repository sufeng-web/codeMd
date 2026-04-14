#pragma once

#include <QWidget>
#include <QLabel>
#include <QPaintEvent>
#include <QKeyEvent>
#include <QPushButton>
#include "../include/IObserver.h"
#include "../Model/GameData.h"
#include "../Model/Fruit.h"

class GameController;

// GameView.h - 游戏视图
class GameView : public QWidget, public IObserver {
     Q_OBJECT
public:
    GameView(GameData* data, QWidget* parent = nullptr);
    
    void setController(GameController* c) { controller = c; }
    void onUpdate() override;  // 实现IObserver接口
    void renderFruit(GameController* controller, Fruit* fruit);
    void showGameOver();
    void showPauseMenu();
    template<typename Func>
    void createMenuBtn(QPushButton*& btn, const QString& img1, const QString& img2, int x, int y, Func&& signal);
protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    
signals:
    void keyPressed(int key);  // 发送输入信号给Controller
    void sigStart();
    void sigPause();
    void sigEnd();
    void sigSetup();
    void sigExit();
    
private:
    GameController* controller = nullptr;
    GameData* gameData;
    QLabel* scoreLabel;
    QLabel* levelLabel;
    QLabel* infoLabel;
    
    QPushButton* btnExit;
    QPushButton* btnSetup;
    QPushButton* btnStart;
    QPushButton* btnPause;
    QPushButton* btnEnd;
};