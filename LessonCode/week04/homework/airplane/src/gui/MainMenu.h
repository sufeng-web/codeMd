#pragma once
#include <QWidget>
#include <QToolButton>

class MainMenu : public QWidget {
    Q_OBJECT
public:
    MainMenu(QWidget *parent = nullptr);
private slots:
    void startGameApple();
    void startGameSpace();
    void startGame3();
    void startGame4();
private:
    QToolButton *btnApple;
    QToolButton *btnSpace;
    QToolButton *btnGame3;
    QToolButton *btnGame4;
    void uiInit();
};


