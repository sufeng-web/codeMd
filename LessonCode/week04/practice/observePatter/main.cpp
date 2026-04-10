#include <iostream>
#include <memory>
#include <vector>
#include <algorithm> 


class KxCombobox;
class KxComboboxObserver;


class KxComboboxSubject
{
public:
    virtual ~KxComboboxSubject() = default;
    virtual void attach(KxComboboxObserver* observer) = 0;
    virtual void detach(KxComboboxObserver* observer) = 0;
    virtual void notifySelectionChanged() = 0; // 重命名原notify，语义更清晰
    virtual void notifyDropdownOpened() = 0;   // 【新增】下拉框展开通知接口
};

// 抽象观察者基类
class KxComboboxObserver
{
public:
    virtual void onSelectionChanged(KxCombobox* source) = 0;
    virtual void onDropdownOpened(KxCombobox* source) = 0; // 【新增】下拉框展开事件接口
    virtual ~KxComboboxObserver();

    // 观察者注册到组合框
    void attachToCombo(KxCombobox* combo);

protected:
    std::vector<KxCombobox*> m_combo;
};

// 具体主题：组合框类
class KxCombobox : public KxComboboxSubject
{
public:
    explicit KxCombobox(std::initializer_list<std::string> items)
        : m_items(items)
    {}

    // 原选择项方法
    void selectItem(int index)
    {
        if (index >= 0 && index < m_items.size())
        {
            m_selectedIndex = index;
            std::cout << "ComboBox selected: " << currentSelection() << "\n";
            notifySelectionChanged(); // 通知选择变更
        }
    }

    // 下拉框展开方法
    void openDropdown()
    {
        std::cout << "ComboBox dropdown opened\n";
        notifyDropdownOpened(); // 通知展开事件
    }

    std::string currentSelection() const
    {
        return (m_selectedIndex != -1) ? m_items[m_selectedIndex] : "";
    }

    
    void attach(KxComboboxObserver* observer) override
    {
        m_observers.push_back(observer);
    }

    // 重写主题接口：分离观察者
    void detach(KxComboboxObserver* observer) override
    {
        auto it = std::remove(m_observers.begin(), m_observers.end(), observer);
        m_observers.erase(it, m_observers.end());
    }

    // 重写主题接口：通知选择变更
    void notifySelectionChanged() override
    {
        for (auto* obs : m_observers)
        {
            obs->onSelectionChanged(this);
        }
    }

    // 重写主题接口：通知下拉框展开
    void notifyDropdownOpened() override
    {
        for (auto* obs : m_observers)
        {
            obs->onDropdownOpened(this);
        }
    }

private:
    std::vector<KxComboboxObserver*> m_observers;
    std::vector<std::string> m_items;
    int m_selectedIndex = -1;
};

// 新观察者：主题切换器 KxThemeSwitcher
class KxThemeSwitcher : public KxComboboxObserver
{
public:
    // 选择变更事件：根据选项切换主题
    void onSelectionChanged(KxCombobox* source) override
    {
        std::string selection = source->currentSelection();
        if (selection == "AdvancedMode")
        {
            std::cout << "[ThemeSwitcher] 切换到高级主题\n";
        }
        else
        {
            std::cout << "[ThemeSwitcher] 切换到默认主题\n";
        }
    }

    // 下拉框展开事件：可按需实现
    void onDropdownOpened(KxCombobox* /*source*/) override
    {
        std::cout << "[ThemeSwitcher] 下拉框已展开，主题切换选项已显示\n";
    }
};

// 原观察者1：选择日志记录器
class KxSelectionLogger : public KxComboboxObserver
{
public:
    void onSelectionChanged(KxCombobox *source) override
    {
        std::cout << "[LOG] selection changed to :" << source->currentSelection() << "\n";
    }

    // 
    void onDropdownOpened(KxCombobox* /*source*/) override
    {
        std::cout << "[LOG] 下拉框已展开\n";
    }
};

// 原观察者2：标签更新器
class KxLabelUpdater : public KxComboboxObserver
{
public:
    void onSelectionChanged(KxCombobox* source) override
    {
        std::cout << "[LABEL] Current selection : " << source->currentSelection() << "\n";
    }

    
    void onDropdownOpened(KxCombobox* /*source*/) override
    {
        std::cout << "[LABEL] 下拉框已展开，当前选项已展示\n";
    }
};

// 原观察者3：依赖控制器
class KxDependentControl : public KxComboboxObserver
{
public:
    void onSelectionChanged(KxCombobox* source) override
    {
        m_enabled = (source->currentSelection() == "Enable") ? true : false;
        std::cout << "[Control] " << (m_enabled ? "Activated" : "Deactivated") << "\n";
    }

   
    void onDropdownOpened(KxCombobox* /*source*/) override
    {
        std::cout << "[Control] 下拉框已展开，控件状态已同步\n";
    }

private:
    bool m_enabled = false;
};

// 观察者析构函数：自动从所有注册的组合框中分离
KxComboboxObserver::~KxComboboxObserver()
{
    for (auto* combox : m_combo)
    {
        combox->detach(this);
    }
}

// 观察者注册到组合框的实现
void KxComboboxObserver::attachToCombo(KxCombobox* combo)
{
    if (combo)
    {
        combo->attach(this);
        m_combo.push_back(combo);
    }
}

// 主函数
int main(void)
{
    // 组合框新增"AdvancedMode"选项，用于测试主题切换
    KxCombobox combo{"Enable", "Disable", "other option", "AdvancedMode"};

    KxLabelUpdater label;
    KxSelectionLogger logger;
    KxDependentControl control;
    KxThemeSwitcher themeSwitcher; 

    // 所有观察者注册到组合框
    label.attachToCombo(&combo);
    logger.attachToCombo(&combo);
    control.attachToCombo(&combo);
    themeSwitcher.attachToCombo(&combo); // 注册新观察者

    std::cout << "===== Test dropdown menu to expand events =====\n";
    combo.openDropdown(); // 触发展开事件

    std::cout << "\n===== Test selection events (including theme switching) =====\n";
    combo.selectItem(0);  // 选择Enable
    combo.selectItem(3);  // 选择AdvancedMode（触发高级主题）
    combo.selectItem(1);  // 选择Disable（切回默认主题）
    system("pause");
    return 0;
}