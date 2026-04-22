#pragma execution_character_set("utf-8")
#include "GameView.h"
#include "../Controller/GameController.h"
#include <QPainter>
#include <QHBoxLayout>
#include <QFont>
#include <QKeyEvent>
#include <qdebug.h>
GameView::GameView(GameData* data, QWidget* parent) : QWidget(parent), gameData(data) 
{
    if (gameData) gameData->attach(this);
    this->setFixedSize(800, 600); // 暂定窗口大小
 
    QString picSet1 = ":/res/Common/Images/PUBLIC_SETUP1.png";
    QString picSet2 = ":/res/Common/Images/PUBLIC_SETUP2.png";
    QString picStart1 = ":/res/Common/Images/PUBLIC_START1.png";
    QString picStart2 = ":/res/Common/Images/PUBLIC_START2.png";
    QString picPause1 = ":/res/Common/Images/PUBLIC_PAUSE1.png";
    QString picPause2 = ":/res/Common/Images/PUBLIC_PAUSE2.png";
    QString picEnd1 = ":/res/Common/Images/PUBLIC_END1.png";
    QString picEnd2 = ":/res/Common/Images/PUBLIC_END2.png";
    QString picExti1 = ":/res/Common/Images/PUBLIC_EXIT1.png";
    QString picExti2 = ":/res/Common/Images/PUBLIC_EXIT2.png";

    this->setWindowTitle("Save Apple");
    scoreLabel = new QLabel("分数: 0", this);
    scoreLabel->setStyleSheet("color: white; font-size: 20px; font-weight: bold; background: rgba(0,0,0,100); border-radius: 5px; padding: 5px;");
    
    levelLabel = new QLabel("难度: 1", this);
    levelLabel->setStyleSheet("color: #FFD700; font-size: 20px; font-weight: bold; background: rgba(0,0,0,100); border-radius: 5px; padding: 5px;");
    
    infoLabel = new QLabel("漏接: 0 / 3", this);
    infoLabel->setStyleSheet("color: #FF6347; font-size: 20px; font-weight: bold; background: rgba(0,0,0,100); border-radius: 5px; padding: 5px;");
    
    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget(scoreLabel);
    layout->addWidget(levelLabel);
    layout->addWidget(infoLabel);
    layout->setAlignment(Qt::AlignTop);
    
    setLayout(layout);
    
    createMenuBtn(btnExit, picExti1, picExti2, 5, 520, &GameView::sigExit);
    createMenuBtn(btnSetup, picSet1, picSet2, 120, 515, &GameView::sigSetup);  // 左 (设置)
    createMenuBtn(btnStart, picStart1, picStart2, 205, 525, &GameView::sigStart);  // 右 (开始)
    createMenuBtn(btnPause, picPause1, picPause2, 160, 480, &GameView::sigPause);  // 上 (暂停)
    createMenuBtn(btnEnd,   picEnd1,   picEnd2,   155, 555, &GameView::sigEnd);    // 下 (结束)
    

    this->setFocusPolicy(Qt::StrongFocus);
}

void GameView::onUpdate() {
    if (!gameData) return;
    
    int gameApplescore = gameData->getScore();
    int gameAppleLevel = gameData->getLevel();
    
    if (gameAppleLevel >= 10) {
        scoreLabel->setText(QString("分数: %1 (已满级)").arg(gameApplescore));//asdas
    } 
    else //j
    {
        scoreLabel->setText(QString("分数: %1 / %2 (至难度%3)").arg(gameApplescore).arg(gameAppleLevel * 100).arg(gameAppleLevel + 1));
    }
    
    levelLabel->setText(QString("难度: %1").arg(gameAppleLevel));
    infoLabel->setText(QString("漏接苹果: %1 / %2 (失败上限)").arg(gameData->getMissed()).arg(gameData->getMaxMisses()));
    
    QWidget::update();
}

void GameView::showGameOver() {
    // show UI...
}

void GameView::showPauseMenu() {
    // show UI...
}
template<typename Func>
void GameView::createMenuBtn(QPushButton*& btn, const QString& img1, const QString& img2, int x, int y, Func&& signal)
{
    btn = new QPushButton(this);
    QPixmap pix(img1);
    int w = pix.isNull() ? 60 : pix.width();
    int h = pix.isNull() ? 60 : pix.height();
    btn->setGeometry(x, y, w, h);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setFocusPolicy(Qt::NoFocus); // 防止按钮抢走键盘焦点
    btn->setStyleSheet(QString("QPushButton { border-image: url(%1); } QPushButton:hover { border-image: url(%2); }").arg(img1, img2));
    connect(btn, &QPushButton::clicked, this, signal);
       
}

void GameView::paintEvent(QPaintEvent* event) 
{
    QPainter p(this);
    
    // 绘制背景图
    QPixmap bg(":/res/Apple/Images/APPLE_BACKGROUND.png");
    if(!bg.isNull())
    {
        p.drawPixmap(this->rect(), bg);
    } 
    else 
    {
        p.fillRect(this->rect(), QColor(135, 206, 235)); // 天空蓝备用背景
    }

    // 绘制篮子与小苹果
    if(this->gameData) 
    {
        QPixmap basket(":/res/Apple/Images/APPLE_BASKET.png");//180*135
        QPixmap smallApple(":/res/Apple/Images/APPLE_SMALL.png");//40*44

        // 假设放在屏幕右下角区域 
        int bx = this->rect().width() - 200;
        int by = this->rect().height() - 180;

        if (!basket.isNull()) 
        {
            p.drawPixmap(bx, by, basket);
        } else 
        {
            p.setBrush(Qt::darkYellow);
            p.drawRect(bx, by, 120, 80);
            qDebug() << "error pic load  !!!\n pic basket error";
        }
        //安全性检查 防止图片加载出错，导致程序崩溃
        if (!smallApple.isNull()) 
        {
            int numApples = gameData->getBasketApples();
            int maxApples = 10; // 最多显示10个(底层4, 中3, 高2, 顶1)
            if (numApples > maxApples) numApples = maxApples;

            // 金字塔排列相对坐标(针对篮子的位置偏移偏量)
            int coords[10][2] = 
            {
                {20, 60}, {45, 60}, {70, 60}, {95, 60}, // 底部4颗
                {32, 40}, {57, 40}, {82, 40},           // 第三层3颗
                {45, 20}, {70, 20},                     // 第二层2颗
                {57, 0}                                 // 最顶层1颗
            };

            int offsetUp = -25; // 让小苹果整体向上浮动,在篮子上方
            for(int i = 0; i < numApples; i++) 
            {
                p.drawPixmap(bx + coords[i][0], by + coords[i][1] + offsetUp, smallApple);
            }
        }
        else
        {
            qDebug() << "error pic load \n smallApple is Null pic load error!\n";
        }
    }
    if (controller) 
    {
        for(const auto& f : controller->getFruits()) 
        {
            QRect bounds = f->getBounds();
            QPixmap appleImg;
            if (f->getState() == FruitState::Broken) 
            {
                // 如果摔碎了，变为失败时的贴图
                appleImg.load(":/res/Apple/Images/APPLE_BAD.png");
            }else 
            {
                // 正常的苹果/果框
                appleImg.load(":/res/Apple/Images/APPLE_NORMAL.png");
            }
            
            if(!appleImg.isNull()) 
            {
                p.drawPixmap(bounds, appleImg);
            }else 
            {
                p.setBrush(f->getState() == FruitState::Broken ? Qt::darkRed : Qt::red);
                p.drawEllipse(bounds);
            }
            // 如果没摔碎，正常展示字母
            if (f->getState() == FruitState::Falling) 
            {
                p.setPen(Qt::white);
                QFont fFont = p.font(); 
                fFont.setPointSize(16); 
                fFont.setBold(true);
                p.setFont(fFont);
                p.drawText(bounds, Qt::AlignCenter, f->getLetter());
            }
        }
    }

    // 绘制游戏结束
    if(gameData && gameData->isGameOver())
    {
        p.setPen(Qt::red);
        QFont f = p.font(); f.setPointSize(40); f.setBold(true);
        p.setFont(f);
        p.drawText(rect(), Qt::AlignCenter, "GAME OVER");
        return;
    }
    //****************************************************************
    // 如果还没结束，但是已经清空（即点了取消等待开始），绘制友好的提示
    //*****************************************************************
    if(gameData && !gameData->isGameOver() && gameData->isPaused() && gameData->getScore() == 0) 
    {
        p.setPen(Qt::white);
        QFont f = p.font(); f.setPointSize(25); f.setBold(true);
        p.setFont(f);
        p.drawText(rect(), Qt::AlignCenter, ">> 点击下方 [开始按钮] 拯救苹果 <<\n可以先点击 [设置] 调整难度喔！");
    }
}

void GameView::renderFruit(GameController* controller, Fruit* fruit) 
{
    // Now handled in paintEvent with the controller's list loop.

}

void GameView::keyPressEvent(QKeyEvent* event) 
{
    emit keyPressed(event->key());
}
