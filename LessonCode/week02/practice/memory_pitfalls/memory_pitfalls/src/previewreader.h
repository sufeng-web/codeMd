#pragma once

#include <cstddef>

class ThemeLoader;

class PreviewReader {
public:
    explicit PreviewReader(ThemeLoader* loader);

    void setLoader(ThemeLoader* loader) { m_loader = loader; }
    ThemeLoader* loader() const { return m_loader; }

    int getPreviewEntryIndex() const;
    bool getPreviewValue(char* dest, std::size_t destSize) const;
    const char* getPreviewValuePointer() const;

private:
    ThemeLoader* m_loader = nullptr;
};
