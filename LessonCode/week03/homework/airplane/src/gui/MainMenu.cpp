#include "MainMenu.h"
#include <QVBoxLayout>
#include <QMessageBox>

MainMenu::MainMenu(QWidget *parent) : QWidget(parent) {
    setWindowTitle("Typing Game");
    uiInit();
    
    connect(btnApple, &QPushButton::clicked, this, &MainMenu::startGameApple);
    connect(btnSpace, &QPushButton::clicked, this, &MainMenu::startGameSpace);
}

void MainMenu::startGameApple() {
    AppleGame *game = new AppleGame;
    game->setAttribute(Qt::WA_DeleteOnClose);
    game->show();
    this->close();
}

void MainMenu::startGameSpace() {
    QMessageBox::information(this, "Info", "Coming Soon");
}

void MainMenu::uiInit()
{
    this->resize(800, 600);

    QVBoxLayout* vLayoutMain = new QVBoxLayout(this);
    QHBoxLayout* hLayoutApple = new QHBoxLayout();
    QHBoxLayout* hLayoutSpace = new QHBoxLayout();
    this->setObjectName("MainMenu");
    this->setAttribute(Qt::WA_StyledBackground, true);

    this->setStyleSheet(R"(
        #MainMenu {
             background: url(:/res/Apple/Images/APPLE_BACKGROUND.png);
             background-position: center;
             background-repeat: no-repeat;
        }
    )");

    QSize btnXY(300, 70);
    btnApple = new QPushButton("Save the Apple", this);
    btnApple->setFixedSize(btnXY);
    btnSpace = new QPushButton("Space Wars", this);
    btnSpace->setFixedSize(btnXY);

    //btnApple->setIcon(QIcon(":/res/Apple/Images/APPLE_BACKGROUND.png"));
    hLayoutApple->addWidget(btnApple);
    hLayoutSpace->addWidget(btnSpace);
    vLayoutMain->addLayout(hLayoutApple);
    vLayoutMain->addLayout(hLayoutSpace);
}

