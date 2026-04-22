# 打字通实战 — 拯救苹果与飞机大战 (MVC重构版)

## 1. 项目概述

本项目为“打字通”实战项目，将原来单一的“拯救苹果”游戏升级为一个综合性游戏平台。项目目前包含两个主要子游戏模块：
- **拯救苹果 (Save Apple)**
- **飞机大战 (Airplane Game)**

玩家在启动主程序后，通过统一的主菜单(`MainMenu`)自由选择进入对应的小游戏。本项目重点对“拯救苹果”模块进行了 **MVC分层架构** 重构，提升了代码的维护性、扩展性及模块间的解耦。

---

## 2. 目录结构

```text
airplane/ (项目工作区根目录)
├── CMakeLists.txt        # CMake构建配置文件
├── src/
│   ├── main.cpp          # 程序入口点
│   ├── gui/
│   │   ├── MainMenu.h
│   │   └── MainMenu.cpp  # 主菜单界面，负责游戏分发与入口
│   └── games/
│       ├── apple/        # ?? "拯救苹果"游戏模块 (MVC重构后代码)
│       │   ├── Model/    # 数据层
│       │   ├── View/     # 视图层
│       │   ├── Controller/ # 控制层
│       │   ├── Config/   # 配置层
│       │   └── include/  # 公共接口(如观察者接口)
│       └── airplane/     # ?? "飞机大战"游戏模块
│           └── AirplaneGame.h # 飞机大战主界面及相关逻辑
└── README.md             # 本说明文档
```

---

## 3. 设计模式与架构说明 (核心)

在本次重构中，我们采用了多种设计模式，旨在解决随着业务逻辑的增多，代码容易出现的“牵一发而动全身”、“界面与逻辑强耦合”的问题。

### 3.1 MVC 架构模式 (Model-View-Controller)
**主要用于：拯救苹果模块 (`src/games/apple/`)**

- **Model (数据层):** `GameData`, `Fruit` 等。
  - **职责:** 专门负责存储、校验与管理游戏核心数据结构和逻辑（如分数、生命值、游戏状态、对象的数学模型坐标等）。它不关心UI如何绘制或按键如何处理。
- **View (视图层):** `GameView`。
  - **职责:** 纯正的显示模块。只负责获取Model中的数据将其绘制到屏幕上，并捕捉玩家的键盘/鼠标事件后对外抛出信号（如将 `keyPressed(int)` 交给Controller处理），不包含任何游戏逻辑判断。
- **Controller (控制层):** `GameController`。
  - **职责:** 核心指挥官。负责接收View抛出的输入事件，驱动定时器(`QTimer`)更新，改变Model的数据，进行碰撞检测等游戏核心进程调度。

**为什么要用MVC？**
在之前的版本中，数据的修改、界面的刷新和按键信号往往混杂在同一个庞大的 `AppleGame/Widget` 文件中。使用MVC模式使得“怎么画(View)”、“有什么(Model)”、与“怎么玩(Controller)”相互独立。比如若以后想要把 Qt View 更换为其他的渲染引擎（如 OpenGL），或者增加网络对战模块，只需替换或修改对应的层，其他层不会受到波及。

---

### 3.2 观察者模式 (Observer Pattern)
**在哪里使用:** `GameData` (被观察者/Subject) 与 `IObserver` 及 `GameView` (观察者/Observer) 之间。

- **实现细节:** `GameData` 内部维护了一个 `std::vector<IObserver*>` 列表，并提供了 `attach`、`detach` 方法。当数据（如分数、生命值、游戏暂停状态等）发生变化时，`GameData` 调用 `notifyObservers()` 通知所有已注册的观察者执行 `onUpdate()`。`GameView` 实现了 `IObserver` 接口，并在接到通知后主动重绘界面。
- **为什么要用？**
  数据层(Model) 不应该直接依赖视图层(View)或者调用界面刷新的具体API。通过观察者接口 `IObserver`，数据和UI实现完美解耦。即使我们添加了另一个显示比分的额外视窗，只需新视窗实现 `IObserver` 并 `attach()` 即可，不要修改原数据类的任何代码，符合“开闭原则”(Open-Closed Principle)。

---

### 3.3 单例模式 (Singleton Pattern)
**在哪里使用:** `GameConfig` 游戏配置类。

- **实现细节:** 类提供了静态工厂方法 `static GameConfig& getInstance()`，并将构造函数私有化 `private: GameConfig();` 且清除了拷贝控制，确保全局环境中仅存在一个配置管理实例。
- **为什么要用？**
  游戏往往有着跨越多个模块的全局配置数据，比如：初始生命数、水果刷新间隔(FPS)、甚至多语言环境字典（国际化文本）。这些配置贯穿 Model、View 和 Controller，如果在各层传递十分繁琐；使用局部全局变量则管理混乱。通过单例模式提供统一的，全局唯一的接入点读取这些配置参数，既避免了重复实例化带来的资源浪费，又方便系统集中管理数值。