#ifndef __MENUVIEW_H_
#define __MENUVIEW_H_
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
#endif