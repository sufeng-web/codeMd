#pragma once

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>

class MenuView : public QWidget {
    Q_OBJECT

public:
    explicit MenuView(QWidget* parent = nullptr);

signals:
    void startBtnClicked();
    void settingsBtnClicked();
    void exitBtnClicked();

private:
    QPushButton* btnStart;
    QPushButton* btnSettings;
    QPushButton* btnExit;
};