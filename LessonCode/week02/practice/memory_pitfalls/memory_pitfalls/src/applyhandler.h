#pragma once

class ThemeLoader;

class ApplyHandler {
public:
    virtual void apply(const ThemeLoader& loader) = 0;
    virtual bool setOutputPath(const char* path);
    virtual ~ApplyHandler() = default; //虚函数所在的类，析构函数也要成虚函数
};

class FileApplyHandler : public ApplyHandler {
public:
    FileApplyHandler();
    virtual ~FileApplyHandler();

    bool setOutputPath(const char* path) override;
    void apply(const ThemeLoader& loader) override;

private:
    struct Impl;
    Impl* m_impl = nullptr;
};

class PreviewHandler : public ApplyHandler {
public:
    PreviewHandler();
    virtual ~PreviewHandler(); //

    bool setOutputPath(const char* path) override;
    void apply(const ThemeLoader& loader) override;

private:
    struct Impl;
    Impl* m_impl = nullptr;
};

class ExportHandler : public ApplyHandler {
public:
    ExportHandler();
    virtual ~ExportHandler();

    bool setOutputPath(const char* path) override;
    void apply(const ThemeLoader& loader) override;

private:
    struct Impl;
    Impl* m_impl = nullptr;
};
