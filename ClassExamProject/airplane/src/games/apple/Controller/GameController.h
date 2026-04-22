#ifndef __GAMECONTROLLER_H_
#define __GAMECONTROLLER_H_

#include <QObject>
#include <QPainter>
#include <QTimer>
#include <vector>
#include <memory>
#include "../Model/GameData.h"
#include "../View/GameView.h"
#include "../Model/Fruit.h"

// GameController.h - 游戏控制器
class GameController : public QObject {
    Q_OBJECT
public:
    GameController(GameData* model, GameView* view);
    ~GameController();

public slots:
    void startGame();
    void pauseGame();
    void resumeGame();
    void restartGame();

    void handleKeyPress(int key);
    void updateGame();  // 定时器调用

private:
    void spawnFruit();
    void checkCollisions();
    void checkGameOver();

    // Stats tracking for logging
    int m_totalPresses = 0;
    int m_correctPresses = 0;
    int m_levelFailures = 0;
    QString m_pressedKeys;
    void resetLevelStats();
    void logLevelStats(bool isGameOver = false);

public:
    const std::vector<std::unique_ptr<Fruit>>& getFruits() const { return fruits; }

private:
    GameData* model;
    GameView* view;
    QTimer* gameTimer;
    QTimer* spawnTimer;
    std::vector<std::unique_ptr<Fruit>> fruits;
};
#endif