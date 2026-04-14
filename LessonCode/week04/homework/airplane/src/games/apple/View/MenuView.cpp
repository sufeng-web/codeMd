#include "MenuView.h"

MenuView::MenuView(QWidget* parent) : QWidget(parent) {
    btnStart = new QPushButton("Start Game", this);
    btnSettings = new QPushButton("Settings", this);
    btnExit = new QPushButton("Exit", this);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(btnStart);
    layout->addWidget(btnSettings);
    layout->addWidget(btnExit);

    // 居中对齐
    layout->setAlignment(Qt::AlignCenter);

    // 将按钮自带的clicked信号直接转发出去
    connect(btnStart, &QPushButton::clicked, this, &MenuView::startBtnClicked);
    connect(btnSettings, &QPushButton::clicked, this, &MenuView::settingsBtnClicked);
    connect(btnExit, &QPushButton::clicked, this, &MenuView::exitBtnClicked);
}