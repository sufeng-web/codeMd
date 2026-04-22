
#include "MenuController.h"
#include <QDebug>
#include "../View/MenuView.h"

MenuController::MenuController(MenuView* view, QObject* parent)
    : QObject(parent), m_view(view)
{
    // 在这里绑定视图界面的信号和控制器的槽函数：
     if (m_view) {
         connect(m_view, SIGNAL(startBtnClicked()), this, SLOT(onStartClicked()));
         connect(m_view, SIGNAL(settingsBtnClicked()), this, SLOT(onSettingsClicked()));
        connect(m_view, SIGNAL(exitBtnClicked()), this, SLOT(onExitClicked()));
     }
}

void MenuController::onStartClicked() {
    qDebug() << "[MenuController] Player clicked start game! Requesting main application to start game...";
    emit startGameRequested();
}

void MenuController::onSettingsClicked() {
    qDebug() << "[MenuController] Player clicked settings! Requesting settings menu...";
    emit showSettingsRequested();
}   

void MenuController::onExitClicked() {
    qDebug() << "[MenuController] Player clicked exit! Requesting to exit the application...";
    emit exitGameRequested();
}