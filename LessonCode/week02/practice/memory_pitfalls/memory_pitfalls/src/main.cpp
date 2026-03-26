#include "themeloader.h"
#include "configview.h"
#include "applyhandler.h"
#include "themedisplay.h"
#include "exportcontext.h"
#include "previewreader.h"
#include "handlerfactory.h"
#include "legacybridge.h"
#include "guardalloc.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef _WIN32
#include <Windows.h>
#include <DbgHelp.h>
#pragma comment(lib, "DbgHelp.lib")
#endif

namespace {

#ifdef _WIN32
const char kCrashDumpName[] = "memorypitfalls_crash.dmp";
constexpr DWORD kMaxPath = 512;

void writeCrashDump(EXCEPTION_POINTERS* pExceptionInfo) {
    char path[kMaxPath] = {};
    DWORD len = GetModuleFileNameA(nullptr, path, kMaxPath);
    if (len == 0 || len >= kMaxPath) {
        return;
    }
    for (DWORD i = len; i > 0; --i) {
        if (path[i - 1] == '\\' || path[i - 1] == '/') {
            path[i] = '\0';
            break;
        }
    }
    std::strncat(path, kCrashDumpName, kMaxPath - 1 - std::strlen(path));
    path[kMaxPath - 1] = '\0';

    HANDLE hFile = CreateFileA(
        path,
        GENERIC_WRITE,
        0,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);
    if (hFile == INVALID_HANDLE_VALUE || !pExceptionInfo) {
        return;
    }
    MINIDUMP_EXCEPTION_INFORMATION mei = {};
    mei.ThreadId = GetCurrentThreadId();
    mei.ExceptionPointers = pExceptionInfo;
    mei.ClientPointers = FALSE;
    MiniDumpWriteDump(
        GetCurrentProcess(),
        GetCurrentProcessId(),
        hFile,
        MiniDumpNormal,
        &mei,
        nullptr,
        nullptr);
    CloseHandle(hFile);
}

LONG WINAPI unhandledExceptionFilter(EXCEPTION_POINTERS* pExceptionInfo) {
    writeCrashDump(pExceptionInfo);
    return EXCEPTION_EXECUTE_HANDLER;
}

LONG WINAPI vectoredExceptionHandler(EXCEPTION_POINTERS* pExceptionInfo) {
    writeCrashDump(pExceptionInfo);
    return EXCEPTION_CONTINUE_SEARCH;
}
#endif


const char kDefaultConfigPath[] = "theme.conf";
const char kDefaultOutputPath[] = "theme_out.conf";
const char kPreviewOutputPath[] = "preview_out.conf";
const char kExportOutputPath[] = "export_out.conf";
constexpr std::size_t kThemeNameBufSize = 64;
constexpr std::size_t kPreviewValueSize = 4;

const char* getConfigPath(int argc, char* argv[]) {
    if (argc > 1) {
        return argv[1];
    }
    return kDefaultConfigPath;
}

bool ensureConfigExists(const char* path) {
    std::FILE* f = std::fopen(path, "r");
    if (f) {
        std::fclose(f);
        return true;
    }
    f = std::fopen(path, "w");
    if (!f) {
        return false;
    }
    std::fprintf(f, "THEME\nname=default\nkey1=val1\npreviewEntry=4\noutputMode=1\n");
    std::fclose(f);
    return true;
}

bool loadThemeConfig(const char* path, ThemeLoader& loader) {
    if (!loader.open(path)) {
        return false;
    }
    if (!loader.load()) {
        return false;
    }
    return true;
}

void setThemeNameFromLoader(ThemeLoader& loader) {
    const char* nameVal = loader.getByKey("name");
    if (!nameVal) {
        nameVal = "default";
    }
    char* nameBuf = new char[kThemeNameBufSize];
    std::strncpy(nameBuf, nameVal, kThemeNameBufSize - 1);
    nameBuf[kThemeNameBufSize - 1] = '\0';
    loader.setThemeNameBuffer(nameBuf);
}

ThemeNameView* setupThemeDisplay(ThemeLoader& loader, ThemeDisplay& display) {
    ThemeNameView* nameView = new ThemeNameView();
    nameView->setThemeName(loader.themeName());
    display.attachView(nameView);
    display.setThemeNamePointer(loader.themeName());
    display.refresh();
    return nameView;
}

void setupValueView(ThemeLoader& loader, ConfigValueView& valueView) {
    const ConfigEntry* firstEntry = loader.getByIndex(0);
    if (firstEntry) {
        valueView.setValue(firstEntry->m_value);
    }
}

void runMainExport(ThemeLoader& loader) {
    ApplyHandler* fileHandler = new FileApplyHandler();
    if (fileHandler->setOutputPath(kDefaultOutputPath)) {
        fileHandler->apply(loader);
        std::cout << "Applied to " << kDefaultOutputPath << "\n";
    }
    delete fileHandler;
}

void submitToLegacy(ThemeLoader& loader, LegacyBridge& bridge, PreviewReader& previewReader) {
    bridge.submitString(loader.themeName());
    char valueBuf[kPreviewValueSize];
    previewReader.getPreviewValue(valueBuf, kPreviewValueSize);
    bridge.submitString(valueBuf);
}

void finishAndRefresh(ThemeLoader* loader, ThemeDisplay& display) {
    display.refresh(); //释放后又调用，改为提前调用后释放
    if (loader) {
        loader->resetThemeCache();
    }
    
}

void runPreviewExport(ThemeLoader& loader, LegacyBridge& bridge) {
    ApplyHandler* previewHandler = new PreviewHandler();
    if (previewHandler->setOutputPath(kPreviewOutputPath)) {
        previewHandler->apply(loader);
    }
    ExportContext exportCtx(&bridge);
    exportCtx.addResource(static_cast<void*>(previewHandler));
   // exportCtx.addResource(static_cast<void*>(previewHandler)); 重复调用
    exportCtx.releaseAll();
}

void runFinalExport(ThemeLoader& loader) {
    int outputMode = 0;
    const char* om = loader.getByKey("outputMode");
    if (om) {
        outputMode = std::atoi(om);
    }
    HandlerFactory factory;
    ApplyHandler* exportHandler = factory.createHandler(outputMode);
    if (exportHandler) {
        validatePointer(exportHandler);
        exportHandler->setOutputPath(kExportOutputPath);
        exportHandler->apply(loader);
    }
    //-----------fix   Memory not released
    delete exportHandler;
    exportHandler = nullptr;
}
   


} // namespace

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetUnhandledExceptionFilter(unhandledExceptionFilter);
    AddVectoredExceptionHandler(1, vectoredExceptionHandler);
#endif

    const char* configPath = getConfigPath(argc, argv);
    if (!ensureConfigExists(configPath)) {
        std::cerr << "Cannot create or read config: " << configPath << "\n";
        return 1;
    }

    ThemeLoader loader;
    if (!loadThemeConfig(configPath, loader)) {
        std::cerr << "Failed to load config: " << configPath << "\n";
        return 1;
    }

    setThemeNameFromLoader(loader);

    ThemeDisplay display;
    ThemeNameView* nameView = setupThemeDisplay(loader, display);

    ConfigValueView valueView;
    setupValueView(loader, valueView);

    runMainExport(loader);

    LegacyBridge bridge;
    PreviewReader previewReader(&loader);
    submitToLegacy(loader, bridge, previewReader);

    bridge.auditLastSubmission();

    finishAndRefresh(&loader, display);

   

    runPreviewExport(loader, bridge);

    runFinalExport(loader);

    delete nameView;
    return 0;
}
