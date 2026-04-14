#pragma once

#include <QObject>

class MenuView;

class MenuController : public QObject {
    Q_OBJECT
public:
    explicit MenuController(MenuView* view, QObject* parent = nullptr);

signals:
    // 发出信号给主应用/窗口，请求场景切换
    void startGameRequested();
    void showSettingsRequested();
    void exitGameRequested();

public slots:
    // 处理来自 MenuView 界面的交互事件
    void onStartClicked();
    void onSettingsClicked();
    void onExitClicked();

private:
    MenuView* m_view;
};