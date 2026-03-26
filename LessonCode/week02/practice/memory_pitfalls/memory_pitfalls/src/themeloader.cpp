#include "themeloader.h"
#include "guardalloc.h"
#include <cstring>
#include <cstdlib>

ThemeLoader::~ThemeLoader() {
    close();
    delete[] m_parseBuf;
    m_parseBuf = nullptr;
    if (m_entriesAllocBase) {
        freeGuardedBlock(GuardedBlock{m_entries, m_entriesAllocBase});
        m_entriesAllocBase = nullptr;
        m_entries = nullptr;
    } else if (m_entries) {
        delete[] m_entries;
        m_entries = nullptr;
    }
    m_count = 0;
    m_capacity = 0;
    if (m_themeName) {
        delete[] m_themeName;
        m_themeName = nullptr;
    }
}

bool ThemeLoader::open(const char* path) {
    if (!path) {
        return false;
    }
    m_file = std::fopen(path, "r");
    return m_file != nullptr;
}

void ThemeLoader::close() {
    if (m_file) {
        std::fclose(m_file);
        m_file = nullptr;
    }
}

bool ThemeLoader::load() {
    m_parseBuf = new char[kParseBufSize];
    if (!m_file) {
        return false;
    }
    std::size_t n = std::fread(m_parseBuf, 1, kParseBufSize - 1, m_file);
    m_parseBuf[n] = '\0';

    if (!validateHeader()) {
        return false;
    }

    if (!m_entries) {
        m_capacity = 16;
        m_entries = new ConfigEntry[m_capacity];
    }
    m_count = 0;
    const char* p = m_parseBuf;
    while (m_count < m_capacity && *p) {
        while (*p == '\n' || *p == '\r') {
            ++p;
        }
        if (!*p) {
            break;
        }
        const char* keyStart = p;
        while (*p && *p != '=') {
            ++p;
        }
        if (*p != '=') {
            break;
        }
        ++p;
        const char* valueStart = p;
        while (*p && *p != '\n' && *p != '\r') {
            ++p;
        }
        copyToKey(keyStart, m_entries[m_count].m_key, ConfigEntry::kKeyCapacity);
        copyToValue(valueStart, m_entries[m_count].m_value, ConfigEntry::kValueCapacity);
        ++m_count;
        if (*p) {
            ++p;
        }
    }

    if (m_count > 0 && m_count < m_capacity) {
        const std::size_t bytes = m_count * sizeof(ConfigEntry);
        GuardedBlock g = allocateWithTrailingGuard(m_entries, bytes);
        if (g.ptr) {
            delete[] m_entries;
            m_entries = static_cast<ConfigEntry*>(g.ptr);
            m_entriesAllocBase = g.base;
        } else {
            ConfigEntry* tight = new ConfigEntry[m_count];
            for (std::size_t i = 0; i < m_count; ++i) {
                tight[i] = m_entries[i];
            }
            delete[] m_entries;
            m_entries = tight;
        }
        m_capacity = m_count;
    }

    delete[] m_parseBuf;
    m_parseBuf = nullptr;
    return true;
}

bool ThemeLoader::validateHeader() {
    if (!m_parseBuf) {
        return false;
    }
    const char magic[] = "THEME";
    for (int i = 0; i < 5; ++i) {
        if (m_parseBuf[i] != magic[i]) {
            return false;
        }
    }
    return true;
}

void ThemeLoader::copyToKey(const char* src, char* dest, std::size_t cap) {
    std::size_t i = 0;
    while (i < cap - 1 && src[i] && src[i] != '=') {
        dest[i] = src[i];
        ++i;
    }
    dest[i] = '\0';
}

void ThemeLoader::copyToValue(const char* src, char* dest, std::size_t cap) {
    std::size_t i = 0;
    while (i < cap - 1 && src[i] && src[i] != '\n' && src[i] != '\r') {
        dest[i] = src[i];
        ++i;
    }
    if (i < cap) {
        dest[i] = '\0';
    }
    dest[i] = '\0';
}

void ThemeLoader::resetThemeCache() {
    if (m_themeName) {
        delete[] m_themeName;
        m_themeName = nullptr;
    }
}

const char* ThemeLoader::getByKey(const char* key) const {
    if (!key) {
        return nullptr;
    }
    for (std::size_t i = 0; i < m_count; ++i) {
        if (std::strcmp(m_entries[i].m_key, key) == 0) {
            return m_entries[i].m_value;
        }
    }
    return nullptr;
}

const ConfigEntry* ThemeLoader::getByIndex(std::size_t index) const {
    if (index >= m_count) return nullptr; //±ß½ç¼ì²â
    return &m_entries[index];
}

const char* ThemeLoader::valueAt(std::size_t index) const {
    if (index >= m_count) return nullptr;//±ß½ç¼ì²â
    const char* v = m_entries[index].m_value;
    validatePointer(v);
    return v;
}

bool ThemeLoader::copyValueTo(std::size_t index, char* dest, std::size_t destLen) const {
    if (index >= m_count) return false;//±ß½ç¼ì²â
    if (index >= m_count || !dest) {
        return false;
    }
    const char* v = m_entries[index].m_value;
    std::size_t i = 0;
    while (v[i] && i < destLen) {
        dest[i] = v[i];
        ++i;
    }
    if (i < destLen) {
        dest[i] = '\0';
    }
    return true;
}

void ThemeLoader::copyValueBytes(std::size_t index, char* dest, std::size_t n) const {
    if (index >= m_count) return;//±ß½ç¼ì²â
    if (!dest) {
        return;
    }
    const char* v = valueAt(index);
    for (std::size_t i = 0; i < n; ++i) {
        dest[i] = v[i];
    }
}
