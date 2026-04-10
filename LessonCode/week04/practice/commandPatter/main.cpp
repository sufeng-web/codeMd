#include <iostream>
#include <string>
#include <vector>
#include <stack>

// 抽象命令基类
class KxCommand
{
public:
    virtual ~KxCommand() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual void redo() = 0; // 新增：重做接口
};

// 接收者：文档类（新增复制/粘贴方法）
class KxDocument
{
public:
    KxDocument(const std::string &content)
        : m_content(content)
    {}

    ~KxDocument() = default;

    void newDocument()
    {
        m_content.clear();
        std::cout << "New Document" << std::endl;
    }

    void saveDocument()
    {
        std::cout << "Save document content: " << m_content << std::endl;
    }

    void appendText(const std::string& text)
    {
        m_content += text;
        std::cout << "Add content: " << text << std::endl;
    }

    std::string getContent() const
    {
        return m_content;
    }

    // 复制功能：记录选中内容到剪贴板
    void copyText(const std::string& selectedText)
    {
        m_clipboard = selectedText;
        std::cout << "Copy content: " << selectedText << std::endl;
    }

    // 粘贴功能：将剪贴板内容追加到文档
    void pasteText()
    {
        m_content += m_clipboard;
        std::cout << "Paste content: " << m_clipboard << std::endl;
    }

    // 获取剪贴板内容（用于撤销/重做）
    std::string getClipboard() const { return m_clipboard; }

private:
    std::string m_content;
    std::string m_clipboard; // 剪贴板，存储复制的内容
};

// 新建命令
class KxNewCommand : public KxCommand
{
public:
    explicit KxNewCommand(KxDocument *pdoc)
        : m_pDocument(pdoc)
    {}

    void execute() override
    {
        m_pPrevContent = m_pDocument->getContent();
        m_pDocument->newDocument();
    }

    void undo() override
    {
        m_pDocument->appendText(m_pPrevContent);
        std::cout << "Undo New Operation" << std::endl;
    }

    void redo() override // 新增：重做新建=再次清空
    {
        m_pPrevContent = m_pDocument->getContent();
        m_pDocument->newDocument();
        std::cout << "Redo New Operation" << std::endl;
    }

private:
    KxDocument* m_pDocument = nullptr;
    std::string m_pPrevContent;
};

// 保存命令
class KxSaveCommand : public KxCommand
{
public:
    explicit KxSaveCommand(KxDocument *pdoc)
        : m_pDocument(pdoc)
    {}

    void execute() override
    {
        m_pDocument->saveDocument();
    }

    void undo() override
    {
        std::cout << "Undo Save Operation" << std::endl;
    }

    void redo() override // 新增：重做保存=再次保存
    {
        m_pDocument->saveDocument();
        std::cout << "Redo Save Operation" << std::endl;
    }

private:
    KxDocument* m_pDocument = nullptr;
};

// 插入命令（支持重做）
class KxInsertCommand : public KxCommand
{
public:
    KxInsertCommand(KxDocument *pdoc, const std::string &text)
        : m_pDocument(pdoc), m_text(text)
    {}

    void execute() override
    {
        m_pDocument->appendText(m_text);
    }

    void undo() override
    {
        auto content = m_pDocument->getContent();
        content = content.substr(0, content.length() - m_text.length());
        m_pDocument->newDocument();
        m_pDocument->appendText(content);
        std::cout << "Undo Insert Operation: " << m_text << std::endl;
    }

    void redo() override // 新增：重做插入=再次追加
    {
        m_pDocument->appendText(m_text);
        std::cout << "Redo Insert Operation: " << m_text << std::endl;
    }

private:
    KxDocument* m_pDocument = nullptr;
    std::string m_text;
};

// 复制命令
class KxCopyCommand : public KxCommand
{
public:
    KxCopyCommand(KxDocument *pdoc, const std::string &selectedText)
        : m_pDocument(pdoc), m_selectedText(selectedText)
    {}

    void execute() override
    {
        m_pDocument->copyText(m_selectedText);
    }

    void undo() override // 复制无状态修改，撤销仅提示
    {
        std::cout << "Undo Copy Operation (copy is stateless)" << std::endl;
    }

    void redo() override // 重做复制=再次复制
    {
        m_pDocument->copyText(m_selectedText);
        std::cout << "Redo Copy Operation" << std::endl;
    }

private:
    KxDocument* m_pDocument = nullptr;
    std::string m_selectedText;
};

// 粘贴命令（支持撤销/重做）
class KxPasteCommand : public KxCommand
{
public:
    explicit KxPasteCommand(KxDocument *pdoc)
        : m_pDocument(pdoc), m_pastedText("")
    {}

    void execute() override
    {
        m_pastedText = m_pDocument->getClipboard(); // 记录粘贴的内容
        m_pDocument->pasteText();
    }

    void undo() override // 撤销粘贴=删除最后追加的内容
    {
        if (m_pastedText.empty()) return;
        auto content = m_pDocument->getContent();
        content = content.substr(0, content.length() - m_pastedText.length());
        m_pDocument->newDocument();
        m_pDocument->appendText(content);
        std::cout << "Undo Paste Operation: " << m_pastedText << std::endl;
    }

    void redo() override // 重做粘贴=再次粘贴
    {
        m_pDocument->pasteText();
        std::cout << "Redo Paste Operation: " << m_pastedText << std::endl;
    }

private:
    KxDocument* m_pDocument = nullptr;
    std::string m_pastedText; // 记录本次粘贴的内容
};

// 调用者：菜单项（新增重做栈、redoLastAction方法）
class KxMenuItem
{
public:
    KxMenuItem(const std::string &label, KxCommand *command)
        :m_label(label), m_pCommand(command)
    {}

    void click()
    {
        std::cout << "Click the menu: " << m_label << std::endl;
        if (m_pCommand)
        {
            m_pCommand->execute();
            s_undoHistory.push(m_pCommand); // 执行后入撤销栈
            // 新操作执行后，清空重做栈（符合常规编辑器逻辑）
            while (!s_redoHistory.empty())
                s_redoHistory.pop();
        }
    }

    // 【新增】重做上一步操作
    static void redoLastAction()
    {
        if (!s_redoHistory.empty())
        {
            auto cmd = s_redoHistory.top();
            cmd->redo();
            s_redoHistory.pop();
            s_undoHistory.push(cmd); // 重做后放回撤销栈
            std::cout << "Redo last action" << std::endl;
        }
        else
        {
            std::cout << "No action to redo" << std::endl;
        }
    }

    // 原撤销方法（修改：撤销后入重做栈）
    static void undoLastAction()
    {
        if (!s_undoHistory.empty())
        {
            auto cmd = s_undoHistory.top();
            cmd->undo();
            s_undoHistory.pop();
            s_redoHistory.push(cmd); // 撤销后入重做栈
            std::cout << "Undo last action" << std::endl;
        }
        else
        {
            std::cout << "No action to undo" << std::endl;
        }
    }

private:
    std::string m_label;
    KxCommand* m_pCommand = nullptr;
    static std::stack<KxCommand*> s_undoHistory; // 撤销栈
    static std::stack<KxCommand*> s_redoHistory; // 【新增】重做栈
};

// 静态成员初始化
std::stack<KxCommand*> KxMenuItem::s_undoHistory;
std::stack<KxCommand*> KxMenuItem::s_redoHistory;

// 主函数（测试所有功能：新建/插入/复制/粘贴/撤销/重做）
int main()
{
    KxDocument document("");

    // 1. 新建文档测试
    KxNewCommand newCommand(&document);
    KxMenuItem newFile("New Document", &newCommand);
    newFile.click();
    newFile.undoLastAction();
    newFile.redoLastAction();

    std::cout << "\n====================\n" << std::endl;

    // 2. 插入文本+复制粘贴测试
    KxInsertCommand insertCommand(&document, "Hello, ");
    KxMenuItem insertItem("Insert Text", &insertCommand);
    insertItem.click();

    KxInsertCommand insertCommand2(&document, "World!");
    KxMenuItem insertItem2("Insert Text 2", &insertCommand2);
    insertItem2.click();

    std::cout << "Current content: " << document.getContent() << std::endl;

    // 复制选中内容
    KxCopyCommand copyCommand(&document, "World!");
    KxMenuItem copyItem("Copy Text", &copyCommand);
    copyItem.click();

    // 粘贴内容
    KxPasteCommand pasteCommand(&document);
    KxMenuItem pasteItem("Paste Text", &pasteCommand);
    pasteItem.click();

    std::cout << "After paste: " << document.getContent() << std::endl;

    // 撤销粘贴
    pasteItem.undoLastAction();
    std::cout << "After undo paste: " << document.getContent() << std::endl;

    // 重做粘贴
    pasteItem.redoLastAction();
    std::cout << "After redo paste: " << document.getContent() << std::endl;

    // 保存文档
    KxSaveCommand saveCommand(&document);
    KxMenuItem saveItem("Save Document", &saveCommand);
    saveItem.click();
    system("pause");
    return 0;
}