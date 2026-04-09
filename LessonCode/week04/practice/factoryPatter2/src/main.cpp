#include <iostream>
#include <memory>

class KxButton
{
public:
    virtual void render() = 0; // 渲染按钮
    virtual void animate() = 0; // 动画效果
    virtual ~KxButton() = default;
};

class KxTextEdit
{
public:
    virtual void draw() = 0; // 绘制文本框
    virtual ~KxTextEdit() = default;
};


class KxStandardButton : public KxButton
{
public:
    void render() override
    {
        std::cout << "[Basic Button]: Square borders | Solid-color background | regular font" << std::endl;
    }
    void animate() override
    {
        std::cout << "[Basic Button Animation]: Color Brightness Switching" << std::endl;
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


class KxModernButton : public KxButton
{
public:
    void render() override
    {
        std::cout << "[Modern Button]: 8px Rounded Corners | Gradient Background | Icons + text" << std::endl;
    }
    void animate() override
    {
        std::cout << "[Modern Button Animation]: Faint Glow Ripple Effect" << std::endl;
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


class KxDarkButton : public KxButton
{
public:
    void render() override
    {
        std::cout << "[Dark Button]: Deep Space Black Background | Cool Glow Text" << std::endl;
    }
    void animate() override
    {
        std::cout << "[Dark Button Animation]: Pulsing Highlight Flash" << std::endl;
    }
};

class KxDarkTextEdit : public KxTextEdit
{
public:
    void draw() override
    {
        std::cout << "[Dark Text Box]: Luminous Border | Low-Brightness Input Hint" << std::endl;
    }
};


class KxButtonFactory
{
public:
    virtual std::unique_ptr<KxButton> createButton() = 0;
    virtual ~KxButtonFactory() = default;
};

class KxTextEditFactory
{
public:
    virtual std::unique_ptr<KxTextEdit> createTextEdit() = 0;
    virtual ~KxTextEditFactory() = default;
};

class KxStandardButtonFactory : public KxButtonFactory
{
public:
    std::unique_ptr<KxButton> createButton() override
    {
        return std::make_unique<KxStandardButton>();
    }
};

class KxBasicTextEditFactory : public KxTextEditFactory
{
public:
    std::unique_ptr<KxTextEdit> createTextEdit() override
    {
        return std::make_unique<KxBasicTextEdit>();
    }
};


class KxModernButtonFactory : public KxButtonFactory
{
public:
    std::unique_ptr<KxButton> createButton() override
    {
        return std::make_unique<KxModernButton>();
    }
};

class KxModernTextEditFactory : public KxTextEditFactory
{
public:
    std::unique_ptr<KxTextEdit> createTextEdit() override
    {
        return std::make_unique<KxMaterialTextEdit>();
    }
};

class KxDarkButtonFactory : public KxButtonFactory
{
public:
    std::unique_ptr<KxButton> createButton() override
    {
        return std::make_unique<KxDarkButton>();
    }
};

class KxDarkTextEditFactory : public KxTextEditFactory
{
public:
    std::unique_ptr<KxTextEdit> createTextEdit() override
    {
        return std::make_unique<KxDarkTextEdit>();
    }
};


int main(void)
{
   
    std::cout << "=== 基础风格组件 ===" << std::endl;
    std::unique_ptr<KxStandardButtonFactory> basicButtonFactory = std::make_unique<KxStandardButtonFactory>();
    auto basicButton = basicButtonFactory->createButton();
    basicButton->render();
    basicButton->animate();

    
    std::cout << "\n=== 现代风格组件 ===" << std::endl;
    std::unique_ptr<KxModernButtonFactory> modernButtonFactory = std::make_unique<KxModernButtonFactory>();
    auto modernButton = modernButtonFactory->createButton();
    modernButton->render();
    modernButton->animate();

   
    std::cout << "\n=== 暗黑风格组件 ===" << std::endl;
    std::unique_ptr<KxDarkButtonFactory> darkButtonFactory = std::make_unique<KxDarkButtonFactory>();
    auto darkButton = darkButtonFactory->createButton();
    darkButton->render();
    darkButton->animate();

    std::unique_ptr<KxDarkTextEditFactory> darkTextEditFactory = std::make_unique<KxDarkTextEditFactory>();
    auto darkTextEdit = darkTextEditFactory->createTextEdit();
    darkTextEdit->draw();
    system("pause");
    return 0;
}