
#include "MainMenu.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include "../games/apple/Model/GameData.h"
#include "../games/apple/View/GameView.h"
#include "../games/apple/Controller/GameController.h"
#include "../games/airplane/AirplaneGame.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif

MainMenu::MainMenu(QWidget *parent) : QWidget(parent) {
    setWindowTitle("金山小游戏");
    uiInit();
    
    connect(btnApple, &QToolButton::clicked, this, &MainMenu::startGameApple);
    connect(btnSpace, &QToolButton::clicked, this, &MainMenu::startGameSpace);
    connect(btnGame3, &QToolButton::clicked, this, &MainMenu::startGame3);
    connect(btnGame4, &QToolButton::clicked, this, &MainMenu::startGame4);
}

void MainMenu::startGameApple() {
    GameData* data = new GameData();
    GameView* view = new GameView(data);
    GameController* controller = new GameController(data, view);
    view->setController(controller);
    controller->startGame();

    view->setAttribute(Qt::WA_DeleteOnClose);
    view->show();
    this->close();
}

void MainMenu::startGameSpace() {
    AirplaneGame* airplaneView = new AirplaneGame();
    airplaneView->setAttribute(Qt::WA_DeleteOnClose);
    airplaneView->show();
    this->close();
}

void MainMenu::startGame3() {
    QMessageBox::information(this, "提示", "生死时速 敬请期待！");
}

void MainMenu::startGame4() {
    QMessageBox::information(this, "提示", "青蛙过河 敬请期待！");
}

void MainMenu::uiInit()
{
    this->resize(900, 600); // 宽一点适配横向4按钮

    this->setObjectName("MainMenu");
    this->setAttribute(Qt::WA_StyledBackground, true);

    // 采用浅色渐变背景（如果没有背景图片，渐变更符合原图样式）
    this->setStyleSheet(R"(
        #MainMenu {
             background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #EAF4FC, stop:0.4 #FFFFFF, stop:1 #EAF4FC);
             /* 如果你想用图片，可以改回: border-image: url(:/res/Common/Images/MAIN_DLG_BG.png); */
        }
        
        QToolButton {
            background-color: white;
            border: 1px solid #DCDCDC;
            border-radius: 5px;
            font-size: 16px;
            font-family: "Microsoft YaHei";
            color: #333333;
            padding-top: 15px;
            padding-bottom: 20px;
        }
        QToolButton:hover {
            border: 1px solid #90C8F6;
            background-color: #F8FBFE;
        }
        QLabel#titleLabel {
            font-family: "Microsoft YaHei";
            font-size: 32px;
            color: #333333;
            margin-top: 50px;
        }
        QLabel#subTitleLabel {
            font-family: "Microsoft YaHei";
            font-size: 16px;
            color: #888888;
            margin-bottom: 50px;
        }
    )");

    QVBoxLayout* vLayoutMain = new QVBoxLayout(this);
    vLayoutMain->setContentsMargins(0, 0, 0, 0);
    
    // 上方标题区
    QLabel* titleLabel = new QLabel("欢迎使用 金山打字游戏", this);
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);
    
    QLabel* subTitleLabel = new QLabel("零基础入门，轻松学打字", this);
    subTitleLabel->setObjectName("subTitleLabel");
    subTitleLabel->setAlignment(Qt::AlignCenter);

    vLayoutMain->addStretch(1);
    vLayoutMain->addWidget(titleLabel);
    vLayoutMain->addWidget(subTitleLabel);

    // 中间游戏按钮水平排列区
    QHBoxLayout* hLayoutGames = new QHBoxLayout();
    hLayoutGames->setAlignment(Qt::AlignCenter);
    hLayoutGames->setSpacing(30);

    auto setupToolBtn = [this](QToolButton*& btn, const QString& text, const QString& iconPath) {
        btn = new QToolButton(this);
        btn->setText(text);
        // 如果有图标可以加上： btn->setIcon(QIcon(iconPath));
        // btn->setIconSize(QSize(64, 64));
        btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        btn->setFixedSize(160, 180);
        btn->setCursor(Qt::PointingHandCursor);
    };

    setupToolBtn(btnGame3, "生死时速", ":/res/Common/Images/PUBLIC_START1.png"); // 可以换成各自真实的icon
    setupToolBtn(btnGame4, "青蛙过河", ":/res/Common/Images/PUBLIC_START1.png");
    setupToolBtn(btnApple, "拯救苹果", ":/res/Common/Images/PUBLIC_START1.png");
    setupToolBtn(btnSpace, "太空大战", ":/res/Common/Images/PUBLIC_START1.png");

    hLayoutGames->addWidget(btnGame3);
    hLayoutGames->addWidget(btnGame4);
    hLayoutGames->addWidget(btnApple);
    hLayoutGames->addWidget(btnSpace);

    vLayoutMain->addLayout(hLayoutGames);
    vLayoutMain->addStretch(2);
}


