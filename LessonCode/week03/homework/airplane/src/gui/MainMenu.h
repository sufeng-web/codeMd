#pragma once
#include <QWidget>
#include <QPushButton>
#include "../games/apple/AppleGame.h"

class MainMenu : public QWidget {
    Q_OBJECT
public:
    MainMenu(QWidget *parent = nullptr);
private slots:
    void startGameApple();
    void startGameSpace();
private:
    QPushButton *btnApple;
    QPushButton *btnSpace;
    void uiInit();
};

