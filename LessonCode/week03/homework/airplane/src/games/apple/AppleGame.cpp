#include "AppleGame.h"
#include <QRandomGenerator>
#include <QDebug>
#include <QMessageBox>
#include "AppleSettingsDialog.h"
AppleGame::AppleGame(QWidget *parent) : QWidget(parent) {
    uiAppleDraw();
    cnnt();

    state = Playing;
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &AppleGame::updateGame);
    timer->start(16); // ~60 FPS
}

void AppleGame::spawnApple()
{
    if (apples.size() >= m_maxApples) return;
    
    Apple a;
    a.x = QRandomGenerator::global()->bounded(100, width() - 100);
    a.y = -50;
    
    // Find a unique letter
    while (true) {
        char c = 'A' + QRandomGenerator::global()->bounded(26);
        bool ok = true;
        //检查生成的字母是否重复，确保生成的苹果在当前屏幕上是唯一的。
        for (const auto& existing : apples)
        {
            if (existing.letter == c)
            {
                ok = false;
                break;
            }
        }
        if (ok)
        {
            a.letter = c;
            break;
        }
    }
    
    apples.push_back(a);
}

void AppleGame::cnnt()
{
    connect(btnExit, &QPushButton::clicked, this, &AppleGame::onbtnExitClick);
    connect(btnEnd, &QPushButton::clicked, this, &AppleGame::onbtnEndClick);
    connect(btnStart, &QPushButton::clicked, this, &AppleGame::onbtnStartClick);
    connect(btnStop, &QPushButton::clicked, this, &AppleGame::onbtnStopClick);
    connect(btnSet, &QPushButton::clicked, this, &AppleGame::onbtnSetClick);
}

void AppleGame::setApple(int level, int successCount, int failCount, int totalPresses, int targetSuccess, int maxApples)
{
    if (level <= 0) return;
    this->m_level = level;
    this->m_successCount = successCount;
    this->m_failCount = failCount;
    this->m_totalPresses = totalPresses;
    this->m_targetSuccess = targetSuccess;
    this->m_maxApples = maxApples;
}

void AppleGame::uiAppleDraw()
{
    setWindowTitle("Save the Apple");
    setFixedSize(800, 600);
    setFocusPolicy(Qt::StrongFocus);
    this->setAttribute(Qt::WA_StyledBackground, true);
    this->setObjectName("AppleWindow");
    this->setStyleSheet(R"(
        #AppleWindow {
            background-image: url(:/res/Apple/Images/APPLE_BACKGROUND.png); 
            background-position: center;
            background-repeat: no-repeat;
        }
    )");
    QSize btnSize(48, 48);
    //上下左右   右边
    QPoint points[5] = {QPoint(150,400),QPoint(150,500),QPoint(100,450),QPoint(200,450),QPoint(50,500)};
    btnExit = new QPushButton(this);
    btnExit->setIcon(QIcon(":/res/Common/Images/PUBLIC_EXIT1.png"));
    btnExit->setIconSize(btnSize);
    btnExit->setFixedSize(btnSize);
    btnExit->move(points[4]);

    btnStop = new QPushButton(this);
    btnStop->setIcon(QIcon(":/res/Common/Images/PUBLIC_PAUSE1.png"));
    btnStop->setIconSize(btnSize);
    btnStop->setFixedSize(btnSize);
    btnStop->move(points[1]);

    btnStart = new QPushButton(this);
    btnStart->setFixedSize(btnSize);
    btnStart->setIcon(QIcon(":/res/Common/Images/PUBLIC_START1.png"));
    btnStart->setIconSize(btnSize);
    btnStart->move(points[0]);

    btnEnd = new QPushButton(this);
    btnEnd->setFixedSize(btnSize);
    btnEnd->setIcon(QIcon(":/res/Common/Images/PUBLIC_END1.png"));
    btnEnd->setIconSize(btnSize);
    btnEnd->move(points[2]);

    btnSet = new QPushButton(this);
    btnSet->setFixedSize(btnSize);
    btnSet->setIcon(QIcon(":/res/Common/Images/PUBLIC_SETUP1.png"));
    btnSet->setIconSize(btnSize);
    btnSet->move(points[3]);

}

void AppleGame::updateGame() {
    if (state != Playing) return;
    
    // 难度等级影响下落速度, speed = 1.0 + level * 0.5
    float speed = 1.0f + m_level * 0.5f;
    
    // 随机生成苹果
    if (QRandomGenerator::global()->bounded(100) < 2) {
        spawnApple();
    }
    
    // 下落与失败判定
    float failY = height() * 0.7f;
    for (auto it = apples.begin(); it != apples.end();) {
        it->y += speed;
        if (it->y >= failY) {
            m_failCount++;
            it = apples.erase(it);
        } else {
            ++it;
        }
    }
    
    update();
}

void AppleGame::keyPressEvent(QKeyEvent *event)
{
    if (state != Playing) return;
    
    int key = event->key();
    if (key >= Qt::Key_A && key <= Qt::Key_Z) {
        m_totalPresses++;
        char target = 'A' + (key - Qt::Key_A);
        
        for (auto it = apples.begin(); it != apples.end(); ++it) {
            if (it->letter == target) {
                m_successCount++;
                apples.erase(it);
                
                if (m_successCount >= m_targetSuccess) {
                    state = End;
                    //QMessageBox::information(this, "Level Cleared", "You have reached the goal!");
                    // Proceed to next level logic
                    m_level++;
                    m_maxApples++;
                    m_targetSuccess += 10;
                    m_successCount = 0;
                    apples.clear();
                    state = Playing;
                }
                break;
            }
        }
    }
}

void AppleGame::onbtnExitClick()
{
    this->close();
}

void AppleGame::onbtnEndClick()
{
}



void AppleGame::onbtnSetClick()
{
    state = Paused;
    AppleSettingsDialog dialog(m_level, m_successCount, m_failCount, m_totalPresses, m_targetSuccess, m_maxApples, this);
    if (dialog.exec() == QDialog::Accepted) {
        setApple(
            dialog.getLevel(),
            dialog.getSuccessCount(),
            dialog.getFailCount(),
            dialog.getTotalPresses(),
            dialog.getTargetSuccess(),
            dialog.getMaxApples()
        );
    }
    state = Playing;
    this->setFocus();
}

void AppleGame::onbtnStopClick()
{
    state = Paused;
}

void AppleGame::onbtnStartClick()
{
    state = Playing;
    // 恢复游戏后确保窗口重新获取焦点，方便键盘继续输入
    this->setFocus();
}

void AppleGame::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    // Draw Fail Line
    p.setPen(Qt::red);
    int failY = height() * 0.7f;
    //p.drawLine(0, failY, width(), failY);
    
    // Draw Apples
    QFont f = p.font();
    f.setPointSize(24);
    p.setFont(f);
    for (const auto& a : apples) 
    {
        QRectF appleRect(a.x, a.y, 40, 40);

        // Draw rectangle background
        p.setBrush(Qt::green);
        p.setPen(Qt::NoPen);
        p.drawImage(appleRect,QImage(":/res/Apple/Images/APPLE_NORMAL.png"));

        // Draw letter
        p.setPen(Qt::white);
        p.drawText(appleRect, Qt::AlignCenter, QString(a.letter));
    }
    
    // Draw Stats
    p.setPen(Qt::red);
    f.setPointSize(13);
    p.setFont(f);
    int score = m_successCount * 10 - m_failCount * 5;
    double accuracy = m_totalPresses == 0 ? 0.0 : ((double)m_successCount / m_totalPresses) * 100.0;
    QString stats = QString("Level: %1 | Score: %2 | Success: %3/%4 | Fail: %5 | Accuracy: %6%")
                        .arg(m_level).arg(score).arg(m_successCount).arg(m_targetSuccess).arg(m_failCount).arg(accuracy, 0, 'f', 1);
    p.drawText(10, 20, stats);
}

