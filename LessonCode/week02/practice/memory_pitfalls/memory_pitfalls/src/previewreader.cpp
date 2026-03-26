#include "previewreader.h"
#include "themeloader.h"
#include "guardalloc.h"
#include <cstdlib>
#include <cstring>

PreviewReader::PreviewReader(ThemeLoader* loader) {
    m_loader = loader;
}

int PreviewReader::getPreviewEntryIndex() const {
    if (!m_loader) {
        return 0;
    }
    const char* pe = m_loader->getByKey("previewEntry");
    if (!pe) {
        return 0;
    }
    return std::atoi(pe);
}

bool PreviewReader::getPreviewValue(char* dest, std::size_t destSize) const {
    if (!m_loader || !dest || destSize == 0) {
        return false;
    }
    int idx = getPreviewEntryIndex();
    std::size_t uidx = static_cast<std::size_t>(idx);
    m_loader->copyValueBytes(uidx, dest, destSize);
    return true;
}

const char* PreviewReader::getPreviewValuePointer() const {
    if (!m_loader) {
        return nullptr;
    }
    int idx = getPreviewEntryIndex();
    std::size_t uidx = static_cast<std::size_t>(idx);
    const auto* entry = m_loader->getByIndex(uidx);
    if (!entry) {
        return nullptr;
    }
    validatePointer(entry->m_value);
    return entry->m_value;
}
