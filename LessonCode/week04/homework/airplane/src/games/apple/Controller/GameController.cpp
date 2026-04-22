#pragma execution_character_set("utf-8")
#include "GameController.h"
#include <QDebug>
#include <QRandomGenerator>
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QSpinBox>
#include <QSlider>
#include <QPushButton>
#include "../GameConfig.h"

GameController::GameController(GameData* model, GameView* view) 
    : QObject(view), model(model), view(view) 
{
    gameTimer = new QTimer(this);
    spawnTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &GameController::updateGame);
    connect(spawnTimer, &QTimer::timeout, this, &GameController::spawnFruit);
    connect(view, &GameView::keyPressed, this, &GameController::handleKeyPress);

    // 连接界面的菜单控制按钮
    connect(view, &GameView::sigStart, this, [this]() 
        {
        qDebug() << "[GameController] 用户点击了界面的 [开始/重新开始] 按钮";
        this->restartGame();
    });
    connect(view, &GameView::sigExit, this, [view](){
        qDebug() << "[GameController] 用户点击了界面的 [退出] 按钮";
        view->close(); 
    });
    connect(view, &GameView::sigEnd, this, [this](){
        qDebug() << "[GameController] 用户点击了界面的 [结束] 按钮";
        this->model->setGameOver(true);
        this->pauseGame();
    });
    connect(view, &GameView::sigPause, this, [this](){
        qDebug() << "[GameController] 用户点击了界面的 [暂停/继续] 按钮";
        if (this->model->isPaused()) this->resumeGame();
        else this->pauseGame();
    });
    connect(view, &GameView::sigSetup, this, [this, view](){
        qDebug() << "[GameController] 用户点击了界面的 [设置] 按钮，弹出设置对话框...\n";
        // 弹出包含速度、数量难度调节的对话框
        QDialog dlg(view);
        dlg.setWindowTitle("游戏设置");
        dlg.setFixedSize(300, 200);
        
        QFormLayout* layout = new QFormLayout(&dlg);
        
        QSpinBox* spinLives = new QSpinBox(&dlg);
        spinLives->setRange(1, 20);
        spinLives->setValue(GameConfig::getInstance().getInitialLives());
        
        QSpinBox* spinMax = new QSpinBox(&dlg);
        spinMax->setRange(1, 10);
        spinMax->setValue(GameConfig::getInstance().getMaxApples());
        
        QSlider* speedSlider = new QSlider(Qt::Horizontal, &dlg);
        speedSlider->setRange(20, 150); // 难度范围 20 - 150 基础下降像素
        speedSlider->setValue(GameConfig::getInstance().getBaseSpeed());
        
        layout->addRow("容错苹果数 (漏接上限):", spinLives);
        layout->addRow("同屏最大苹果数 (基础):", spinMax);
        layout->addRow("苹果下坠速度 (基数):", speedSlider);
        
        QPushButton* btnOk = new QPushButton("确定并应用", &dlg);
        layout->addWidget(btnOk);
        connect(btnOk, &QPushButton::clicked, &dlg, &QDialog::accept);
        
        if (dlg.exec() == QDialog::Accepted) {
            GameConfig::getInstance().setInitialLives(spinLives->value());
            GameConfig::getInstance().setMaxApples(spinMax->value());
            GameConfig::getInstance().setBaseSpeed(speedSlider->value());
            
            // 可视情况决定是否立即应用新参数
            this->model->setMaxMisses(spinLives->value());
        }
    });
}

void GameController::startGame() 
{
    model->resetScore();
    model->resetMissed();
    model->setMaxMisses(GameConfig::getInstance().getInitialLives());
    model->setLevel(1);
    model->setGameOver(false);
    model->setPaused(false);
    fruits.clear();
    gameTimer->start(16); // ~60fps
    spawnTimer->start(1000); // 1s
}

void GameController::pauseGame() {
    gameTimer->stop();
    spawnTimer->stop();
    model->setPaused(true);
}

void GameController::resumeGame() {
    gameTimer->start();
    spawnTimer->start();
    model->setPaused(false);
}

void GameController::restartGame() {
    fruits.clear();
    startGame();
}

void GameController::handleKeyPress(int key) 
{
    qDebug() << "[GameController] 用户按下了按键代码:" << key << "，对应字符/名称:" << QKeySequence(key).toString();

    if (model->isGameOver() || model->isPaused()) return;
    
    // 如果按下的是 A-Z
    if (key >= Qt::Key_A && key <= Qt::Key_Z) {
        QString keyStr = QKeySequence(key).toString();
        qDebug() << "Pressed key:" << keyStr; // Debug output for pressed key
        // 查找屏幕中是否有此字母的苹果
        for (auto it = fruits.begin(); it != fruits.end(); ++it) {
            if ((*it)->getState() == FruitState::Falling && (*it)->getLetter() == keyStr) {
                model->addScore((*it)->getScoreValue());
                model->addBasketApple();
                // TODO: 在这里可以添加击中音效
                fruits.erase(it);
                
                // 检查是否满足等级晋升条件（每100分升1级）
                int currentLevel = model->getLevel();
                if (currentLevel < 10 && model->getScore() >= currentLevel * 100) {
                    pauseGame();
                    
                    QMessageBox msgBox(view);
                    msgBox.setWindowTitle("恭喜过关");
                    msgBox.setText(QString("太棒了！您已顺利通过难度 %1！\n\n是否准备好迎接更大的挑战，进入 难度 %2 ？")
                                   .arg(currentLevel).arg(currentLevel + 1));
                    msgBox.setIcon(QMessageBox::Information);
                    
                    QPushButton *btnYes = msgBox.addButton("继续挑战", QMessageBox::YesRole);
                    QPushButton *btnNo = msgBox.addButton("结束游戏", QMessageBox::NoRole);
                    msgBox.exec();

                    if (msgBox.clickedButton() == btnYes) {
                        model->setLevel(currentLevel + 1);
                        model->clearBasketApples();
                        fruits.clear(); // 清空屏幕上的旧苹果，重新开始掉落下个难度的苹果
                        resumeGame();
                    } else {
                        // 选择否：不继续，直接回到等待开始的空闲界面
                        fruits.clear();
                        model->resetScore();
                        model->setLevel(1);
                        model->resetMissed();
                        model->clearBasketApples();
                        model->setGameOver(false);
                        pauseGame(); // 停在 Idle 状态
                        view->update();
                    }
                }
                
                break; // 击中一个不继续匹配
            }
        }
    }
}

void GameController::updateGame() 
{
    for (auto& f : fruits) 
    {
        f->update(0.016f);  // 使用标准的 16ms 进行平滑下落更新，而不是疯狂加速
    }
    this->checkCollisions();
    this->checkGameOver();
    view->QWidget::update(); // 通知画面需要再次刷新渲染
}

void GameController::spawnFruit() {
    if (model->isGameOver() || model->isPaused()) return;

    int level = model->getLevel();
    // 难度越高，同屏苹果最大数量增加：难度1最大数量为基础设置，之后每升2级+1
    int maxApples = GameConfig::getInstance().getMaxApples() + (level / 2);
    if (fruits.size() >= maxApples) return; // 屏幕最大限制由设定和等级动态决定

    int startX = QRandomGenerator::global()->bounded(50, 750); // 随机X（留边距）
    
    // 速度随等级（或者分数）挂钩，同时受到设置控制的基础速度影响
    int speed = GameConfig::getInstance().getBaseSpeed() + (level * 15); // 用户设置的下坠速度基数 + 难度加成

    // 随机一个未占用的字母
    QString randLetter;
    bool exists;
    do {
        int l = QRandomGenerator::global()->bounded(0, 26);
        randLetter = QChar('A' + l);
        exists = false;
        for (auto& f : fruits) {
            if(f->getLetter() == randLetter) exists = true;
        }
    } while(exists);

    fruits.push_back(std::make_unique<Fruit>(FruitType::Apple, QPoint(startX, -50), randLetter, speed));
}

void GameController::checkCollisions() 
{
    if (model->isGameOver()) return;

    auto it = fruits.begin();
    while (it != fruits.end()) {
        auto& fruit = *it;
        
        // 需求：落到底部 70% 判定为失败并播碎裂图
        if (fruit->getState() == FruitState::Falling && fruit->getPosition().y() > 600 * 0.7) 
        { 
            fruit->setBroken();
            model->addMissed();
            // TODO: 在此处播放失败音效
            ++it;
        } else if (fruit->isExpired()) 
        {
            it = fruits.erase(it); // 只有动画播放完之后才会从界面抹除
        } else 
        {
            ++it;
        }
    }
}

void GameController::checkGameOver() 
{
    if (!model->isGameOver() && model->getMissed() >= model->getMaxMisses()) 
    {
        model->setGameOver(true);
        pauseGame();
        
        // 游戏结束触发重新开始 / 返回空闲状态 的询问弹窗
        QMessageBox msgBox(view);
        msgBox.setWindowTitle("游戏结束");
        msgBox.setText(QString("通关失败！\n\n您达到了 %1 漏接上限，最终得分为: %2 分。\n最高生存到了难度等级: %3。\n\n是否继续重新开始游戏？")
                       .arg(model->getMaxMisses()).arg(model->getScore()).arg(model->getLevel()));
        msgBox.setIcon(QMessageBox::Question);
        
        QPushButton *btnYes = msgBox.addButton("继续游戏", QMessageBox::YesRole);
        QPushButton *btnNo = msgBox.addButton("结束等待", QMessageBox::NoRole);
        
        msgBox.exec();
        
        if (msgBox.clickedButton() == btnYes) 
        {
            // 是 => 点击是否继续游戏，是--转到开始界面 自动开始游戏
            this->startGame();
        } else 
        {
            // 否 => 转到开始界面 不开始游戏，等待玩家点击开始
            fruits.clear();
            model->resetScore();
            model->setLives(GameConfig::getInstance().getInitialLives());
            model->setGameOver(false);
            this->pauseGame();  // 保持暂停状态，即“开始前的等待(Idle)”状态
            view->update(); // 刷新画面清空苹果残影
        }
    }
}
