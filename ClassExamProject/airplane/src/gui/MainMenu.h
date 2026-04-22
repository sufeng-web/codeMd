#ifndef __MAINMENU_H_
#define __MAINMENU_H_
#include <QWidget>
#include <QToolButton>

/**
 * @brief 整个游戏项目的【主启动界面入口】 (GUI 层)
 * 
 * 职责：这是一个独立的大厅。
 * 1. 负责当玩家双击图标启动游戏后（`main.cpp` 中调用），展示一个有着多个模块可选的大厅。
 * 2. 它作为“枢钮中心”，不掺合每个小游戏的具体逻辑（MVC都分开）。
 * 3. 当玩家挑选点击某个小游戏时，MainMenu 把这个对应的 MVC 给完全组装(new)并推到屏幕上，之后自行关闭。
 */
class MainMenu : public QWidget {
    Q_OBJECT
public:
    /**
     * @brief 构造函数：大厅的加载器
     * @details 初始化界面（uiInit），并绑定4个游戏选择大图标和对应的启动响应槽（connect）。
     */
    MainMenu(QWidget *parent = nullptr);

private slots:
    // ==========================================
    // 各个小游戏的独立引导入口与控制器 (工厂模式雏形)
    // ==========================================
    
    /**
     * @brief 苹果打字游戏入口！
     * @details 用户点击图标后：
     * 1. new GameData (数据源)
     * 2. new GameView (视图源)
     * 3. new GameController(data, view) (逻辑室相互绑定)
     * 4. show 视图开始游戏，close 大厅。
     */
    void startGameApple();
    
    /**
     * @brief 太空射击游戏入口
     * @details 直接生成 AirplaneGame 并关闭大厅自身。
     */
    void startGameSpace();
    
    // 两个空壳按钮，点击弹框“敬请期待”提示占位符。
    void startGame3();
    void startGame4();

private:
    QToolButton *btnApple; // 拯救苹果·巨型图像文本并排按钮组
    QToolButton *btnSpace; // 太空大战
    QToolButton *btnGame3; // 生死时速
    QToolButton *btnGame4; // 青蛙过河

    /**
     * @brief 执行纯粹的大厅界面排版 (QHBoxLayout 与 QVBoxLayout)
     * @details 将四个方形按钮居中加入布局，设定大厅的宽宽比例 (resize(900, 600))，
     * 以及运用 Qt Style Sheet 设计优美的过渡色背景、按钮字体样式。
     */
    void uiInit();
};

#endif
