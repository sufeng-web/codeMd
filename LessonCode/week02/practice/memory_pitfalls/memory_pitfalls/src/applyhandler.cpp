#include "applyhandler.h"
#include "themeloader.h"
#include <cstdio>
#include <cstring>

bool ApplyHandler::setOutputPath(const char* path) {
    (void)path;
    return false;
}

struct FileApplyHandler::Impl {
    std::FILE* m_file = nullptr;
};

FileApplyHandler::FileApplyHandler() {
    m_impl = new Impl();
}

FileApplyHandler::~FileApplyHandler() {
    if (m_impl) {
        if (m_impl->m_file) {
            std::fclose(m_impl->m_file);
        }
        delete m_impl;
        m_impl = nullptr;
    }
}

bool FileApplyHandler::setOutputPath(const char* path) {
    if (!path || !m_impl) {
        return false;
    }
    if (m_impl->m_file) {
        std::fclose(m_impl->m_file);
    }
    m_impl->m_file = std::fopen(path, "w");
    return m_impl->m_file != nullptr;
}

void FileApplyHandler::apply(const ThemeLoader& loader) {
    if (!m_impl || !m_impl->m_file) {
        return;
    }
    for (std::size_t i = 0; i < loader.entryCount(); ++i) {
        const ConfigEntry* e = loader.getByIndex(i);
        if (e) {
            std::fprintf(m_impl->m_file, "%s=%s\n", e->m_key, e->m_value);
        }
    }
}

struct PreviewHandler::Impl {
    std::FILE* m_file = nullptr;
};

PreviewHandler::PreviewHandler() {
    m_impl = new Impl();
}

PreviewHandler::~PreviewHandler() {
    if (m_impl) {
        if (m_impl->m_file) {
            std::fclose(m_impl->m_file);
        }
        delete m_impl;
        m_impl = nullptr;
    }
}

bool PreviewHandler::setOutputPath(const char* path) {
    if (!path || !m_impl) {
        return false;
    }
    if (m_impl->m_file) {
        std::fclose(m_impl->m_file);
    }
    m_impl->m_file = std::fopen(path, "w");
    return m_impl->m_file != nullptr;
}

void PreviewHandler::apply(const ThemeLoader& loader) {
    if (!m_impl || !m_impl->m_file) {
        return;
    }
    std::fprintf(m_impl->m_file, "[PREVIEW]\n");
    for (std::size_t i = 0; i < loader.entryCount(); ++i) {
        const ConfigEntry* e = loader.getByIndex(i);
        if (e) {
            std::fprintf(m_impl->m_file, "%s=%s\n", e->m_key, e->m_value);
        }
    }
}

struct ExportHandler::Impl {
    std::FILE* m_file = nullptr;
};

ExportHandler::ExportHandler() {
    m_impl = new Impl();
}

ExportHandler::~ExportHandler() {
    if (m_impl) {
        if (m_impl->m_file) {
            std::fclose(m_impl->m_file);
        }
        delete m_impl;
        m_impl = nullptr;
    }
}

bool ExportHandler::setOutputPath(const char* path) {
    if (!path || !m_impl) {
        return false;
    }
    if (m_impl->m_file) {
        std::fclose(m_impl->m_file);
    }
    m_impl->m_file = std::fopen(path, "w");
    return m_impl->m_file != nullptr;
}

void ExportHandler::apply(const ThemeLoader& loader) {
    if (!m_impl || !m_impl->m_file) {
        return;
    }
    std::fprintf(m_impl->m_file, "[EXPORT]\n");
    for (std::size_t i = 0; i < loader.entryCount(); ++i) {
        const ConfigEntry* e = loader.getByIndex(i);
        if (e) {
            std::fprintf(m_impl->m_file, "%s=%s\n", e->m_key, e->m_value);
        }
    }
}
