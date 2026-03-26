#include "themeconfig.h"
#include "themeloader.h"
#include "configview.h"
#include "applyhandler.h"
#include <cstdio>
#include <cstring>
#include <memory>

ThemeConfigPipeline::ThemeConfigPipeline() {
    std::strncpy(m_pathBuf, "theme_demo.conf", kPathCapacity - 1);
    m_pathBuf[kPathCapacity - 1] = '\0';
}

void ThemeConfigPipeline::setConfigPath(const char* path) {
    if (!path) {
        return;
    }
    std::strncpy(m_pathBuf, path, kPathCapacity - 1);
    m_pathBuf[kPathCapacity - 1] = '\0';
}

bool ThemeConfigPipeline::writeDemoConfig() const {
    std::FILE* f = std::fopen(m_pathBuf, "w");
    if (!f) {
        return false;
    }
    std::fprintf(f, "THEME\nname=default\nkey1=val1\n");
    std::fclose(f);
    return true;
}

bool ThemeConfigPipeline::runSafeImpl() {
    ThemeLoader loader;
    if (!loader.open(m_pathBuf)) {
        return false;
    }
    if (!loader.load()) {
        return false;
    }

    std::size_t n = loader.entryCount();
    if (n > 0) {
        const ConfigEntry* e = loader.getByIndex(0);
        (void)e;
    }

    char* nameBuf = new char[32];
    std::strncpy(nameBuf, "default", 31);
    nameBuf[31] = '\0';
    loader.setThemeNameBuffer(nameBuf);

    ThemeNameView nameView;
    nameView.setThemeName(loader.themeName());
    (void)nameView.getThemeName();

    ConfigValueView valueView;
    if (n > 0) {
        valueView.setValue(loader.getByIndex(0)->m_value);
    }
    (void)valueView.getValue();

    std::unique_ptr<FileApplyHandler> fileHandler(new FileApplyHandler());
    fileHandler->setOutputPath("theme_out.conf");
    fileHandler->apply(loader);

    std::remove(m_pathBuf);
    std::remove("theme_out.conf");
    return true;
}

bool ThemeConfigPipeline::runSafe() {
    if (!writeDemoConfig()) {
        return false;
    }
    return runSafeImpl();
}
