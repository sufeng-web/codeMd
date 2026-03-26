#pragma once

#include <cstddef>

class ThemeConfigPipeline {
public:
    ThemeConfigPipeline();

    void setConfigPath(const char* path);
    const char* configPath() const { return m_pathBuf; }

    bool runSafe();

private:
    bool writeDemoConfig() const;
    bool runSafeImpl();

    static constexpr std::size_t kPathCapacity = 256;
    char m_pathBuf[kPathCapacity];
};
