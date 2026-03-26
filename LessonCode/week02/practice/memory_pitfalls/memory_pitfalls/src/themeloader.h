#pragma once

#include <cstddef>
#include <cstdio>

struct ConfigEntry {
    static constexpr std::size_t kKeyCapacity = 32;
    static constexpr std::size_t kValueCapacity = 64;
    char m_key[kKeyCapacity];
    char m_value[kValueCapacity];
};

class ThemeLoader {
public:
    ThemeLoader() = default;
    ~ThemeLoader();

    bool open(const char* path);
    void close();
    bool load();

    std::size_t entryCount() const { return m_count; }
    const char* getByKey(const char* key) const;
    const ConfigEntry* getByIndex(std::size_t index) const;
    const char* valueAt(std::size_t index) const;
    bool copyValueTo(std::size_t index, char* dest, std::size_t destLen) const;
    void copyValueBytes(std::size_t index, char* dest, std::size_t n) const;

    const char* themeName() const { return m_themeName; }
    void setThemeNameBuffer(char* buf) { m_themeName = buf; }
    void resetThemeCache();

private:
    bool validateHeader();
    void copyToKey(const char* src, char* dest, std::size_t cap);
    void copyToValue(const char* src, char* dest, std::size_t cap);

    std::FILE* m_file = nullptr;
    ConfigEntry* m_entries = nullptr;
    void* m_entriesAllocBase = nullptr;
    std::size_t m_count = 0;
    std::size_t m_capacity = 0;
    char* m_parseBuf = nullptr;
    static constexpr std::size_t kParseBufSize = 4096;
    char* m_themeName = nullptr;
};
