#include <iostream>
#include <memory>

/* 
抽像产品
*KxButton---------KxModernButton    KxStandardButton 
*KxTextEdit-------KxBasicTextEdit   KxMaterialTextEdit
*KxCheckBox-------KxBasicCheckBox   KxModernCheckBox
*/
// 抽象产品：按钮
class KxButton
{
public:
    virtual void render() = 0;  // 渲染按钮
    virtual void animate() = 0; // 动画效果
    virtual ~KxButton() = default;
};

// 抽象产品：文本框 
class KxTextEdit
{
public:
    virtual void draw() = 0; // 绘制文本框
    virtual ~KxTextEdit() = default;
};

//抽象产品：复选框
class KxCheckBox
{
public:
    virtual void render() = 0; // 渲染复选框
    virtual ~KxCheckBox() = default;
};

//  具体产品：基础风格按钮 
class KxStandardButton : public KxButton
{
public:
    void render() override
    {
        std::cout << "[Basic Button] Square Borders | Solid Color Background | Regular Font" << std::endl;
    }
    void animate() override
    {
        std::cout << "[Basic Button Animation]: Color Brightness Switching" << std::endl;
    }
};

// 具体产品：基础风格文本框
class KxBasicTextEdit : public KxTextEdit
{
public:
    void draw() override
    {
        std::cout << "[Basic Text Box]: Single-line Border | White Background | System Font" << std::endl;
    }
};

// 具体产品：基础风格复选框 
class KxBasicCheckBox : public KxCheckBox
{
public:
    void render() override
    {
        std::cout << "[Basic CheckBox] Square Box | Check Mark" << std::endl;
    }
};

// 具体产品：现代风格按钮
class KxModernButton : public KxButton
{
public:
    void render() override
    {
        std::cout << "[Modern Button]: 8px Rounded Corners | Gradient Background" << std::endl;
    }
    void animate() override
    {
        std::cout << "[Modern Button Animation]: Faint Glow Ripple Effect" << std::endl;
    }
};

//  具体产品：现代风格文本框 
class KxMaterialTextEdit : public KxTextEdit
{
public:
    void draw() override
    {
        std::cout << "[Textured Text Box]: Hover Shadow | Dynamic Underline | Floating Label" << std::endl;
    }
};

// 具体产品：现代风格复选框 
class KxModernCheckBox : public KxCheckBox
{
public:
    void render() override
    {
        std::cout << "[Modern CheckBox] Animated Transition | Rounded Design" << std::endl;
    }
};

//风格枚举 
enum class KxStyleType
{
    BasicStyleType,
    ModernStyleType,
};

//  核心：UI简单工厂 
class UiSimpleFactory
{
public:
    // 工厂方法：创建按钮
    static std::unique_ptr<KxButton> createButton(KxStyleType style)
    {
        switch (style)
        {
        case KxStyleType::BasicStyleType:
            return std::make_unique<KxStandardButton>();
        case KxStyleType::ModernStyleType:
            return std::make_unique<KxModernButton>();
        default:
            return nullptr;
        }
    }

    // 工厂方法：创建文本框
    static std::unique_ptr<KxTextEdit> createTextEdit(KxStyleType style)
    {
        switch (style)
        {
        case KxStyleType::BasicStyleType:
            return std::make_unique<KxBasicTextEdit>();
        case KxStyleType::ModernStyleType:
            return std::make_unique<KxMaterialTextEdit>();
        default:
            return nullptr;
        }
    }

    // 工厂方法：创建复选框
    static std::unique_ptr<KxCheckBox> createCheckBox(KxStyleType style)
    {
        switch (style)
        {
        case KxStyleType::BasicStyleType:
            return std::make_unique<KxBasicCheckBox>();
        case KxStyleType::ModernStyleType:
            return std::make_unique<KxModernCheckBox>();
        default:
            return nullptr;
        }
    }
};


int main()
{
    // 原有按钮测试
    auto basicButton = UiSimpleFactory::createButton(KxStyleType::BasicStyleType);
    basicButton->render();
    basicButton->animate();

    std::cout << "\n--- Checkbox component testing ---" << std::endl;
    // 基础风格复选框测试
    auto basicCheckBox = UiSimpleFactory::createCheckBox(KxStyleType::BasicStyleType);
    basicCheckBox->render();

    // 现代风格复选框测试
    auto modernCheckBox = UiSimpleFactory::createCheckBox(KxStyleType::ModernStyleType);
    modernCheckBox->render();

    system("pause");
    return 0;
}