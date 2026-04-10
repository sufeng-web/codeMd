#include <iostream>
#include <memory>

// 按钮抽象
class KxButton
{
public:
    virtual void render() = 0;
    virtual void animate() = 0;
    virtual ~KxButton() = default;
};

// 文本框抽象
class KxTextEdit
{
public:
    virtual void draw() = 0;
    virtual ~KxTextEdit() = default;
};


class KxCheckBox
{
public:
    virtual void check() = 0;
    virtual ~KxCheckBox() = default;
};


class KxStandardButton : public KxButton
{
public:
    void render() override
    {
        std::cout << "[Basic Button]: Square Borders | Solid Color Background | Regular Font" << std::endl;
    }
    void animate() override
    {
        std::cout << "Basic Button Animation: Color Brightness Switching" << std::endl;
    }
};

class KxBasicTextEdit : public KxTextEdit
{
public:
    void draw() override
    {
        std::cout << "[Basic Text Box]: Single-line Border | White Background | System Font" << std::endl;
    }
};

class KxBasicCheckBox : public KxCheckBox
{
public:
    void check() override
    {
        std::cout << "[Basic CheckBox]: Box selection | Check mark" << std::endl;
    }
};

class KxModernButton : public KxButton
{
public:
    void render() override
    {
        std::cout << "[Modern Button]: 8px Rounded Corners | Gradient Background" << std::endl;
    }
    void animate() override
    {
        std::cout << "Modern Button Animation: Faint Glow Ripple Effect" << std::endl;
    }
};

class KxMaterialTextEdit : public KxTextEdit
{
public:
    void draw() override
    {
        std::cout << "[Textured Text Box]: Hover Shadow | Dynamic Underline | Floating Label" << std::endl;
    }
};

class KxModernCheckBox : public KxCheckBox
{
public:
    void check() override
    {
        std::cout << "[Modern CheckBox]: Animated transition | rounded corner design" << std::endl;
    }
};


class KxFlatButton : public KxButton
{
public:
    void render() override
    {
        std::cout << "[Flat Button]: Borderless | Solid color background | Flat icon" << std::endl;
    }
    void animate() override
    {
        std::cout << "Flat Button Animation: Click to change color | Minimal transition" << std::endl;
    }
};

class KxFlatTextEdit : public KxTextEdit
{
public:
    void draw() override
    {
        std::cout << "[Flat TextEdit]: Borderless | Light underline | Minimal input box" << std::endl;
    }
};

class KxFlatCheckBox : public KxCheckBox
{
public:
    void check() override
    {
        std::cout << "[Flat CheckBox]: Flat selection | Minimal line style" << std::endl;
    }
};

class KxUIFactory
{
public:
    virtual std::unique_ptr<KxButton> createButton() = 0;
    virtual std::unique_ptr<KxTextEdit> createTextEdit() = 0;
    virtual std::unique_ptr<KxCheckBox> createCheckBox() = 0;
    virtual ~KxUIFactory() = default;
};

// 基础风格工厂
class KxBasicFactory : public KxUIFactory
{
public:
    std::unique_ptr<KxButton> createButton() override
    {
        return std::make_unique<KxStandardButton>();
    }
    std::unique_ptr<KxTextEdit> createTextEdit() override
    {
        return std::make_unique<KxBasicTextEdit>();
    }
    std::unique_ptr<KxCheckBox> createCheckBox() override
    {
        return std::make_unique<KxBasicCheckBox>();
    }
};

// 现代风格工厂
class KxModernFactory : public KxUIFactory
{
public:
    std::unique_ptr<KxButton> createButton() override
    {
        return std::make_unique<KxModernButton>();
    }
    std::unique_ptr<KxTextEdit> createTextEdit() override
    {
        return std::make_unique<KxMaterialTextEdit>();
    }
    std::unique_ptr<KxCheckBox> createCheckBox() override
    {
        return std::make_unique<KxModernCheckBox>();
    }
};

// 扁平化风格工厂
class KxFlatFactory : public KxUIFactory
{
public:
    std::unique_ptr<KxButton> createButton() override
    {
        return std::make_unique<KxFlatButton>();
    }
    std::unique_ptr<KxTextEdit> createTextEdit() override
    {
        return std::make_unique<KxFlatTextEdit>();
    }
    std::unique_ptr<KxCheckBox> createCheckBox() override
    {
        return std::make_unique<KxFlatCheckBox>();
    }
};


int main(void)
{
    // 基础风格
    KxBasicFactory basicFactory;
    auto btn1 = basicFactory.createButton();
    auto edit1 = basicFactory.createTextEdit();
    auto check1 = basicFactory.createCheckBox();
    btn1->render();
    edit1->draw();
    check1->check();

    std::cout << "----------------" << std::endl;

    // 现代风格
    KxModernFactory modernFactory;
    auto btn2 = modernFactory.createButton();
    auto edit2 = modernFactory.createTextEdit();
    auto check2 = modernFactory.createCheckBox();
    btn2->render();
    edit2->draw();
    check2->check();

    std::cout << "----------------" << std::endl;

    // 扁平化风格
    KxFlatFactory flatFactory;
    auto btn3 = flatFactory.createButton();
    auto edit3 = flatFactory.createTextEdit();
    auto check3 = flatFactory.createCheckBox();
    btn3->render();
    edit3->draw();
    check3->check();
    system("pause");
    return 0;
}