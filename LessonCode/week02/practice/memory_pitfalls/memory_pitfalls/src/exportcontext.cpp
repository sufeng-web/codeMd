#include "exportcontext.h"
#include "applyhandler.h"
#include "legacybridge.h"
#include <cstddef>

ExportContext::ExportContext(LegacyBridge* bridge) {
    m_bridge = bridge;
}

void ExportContext::registerHandler(ApplyHandler* handler) {
    if (handler && m_count < kMaxResources) {
        m_resources[m_count] = static_cast<void*>(handler);
        ++m_count;
    }
}

void ExportContext::addResource(void* resource) {
    if (resource && m_count < kMaxResources) {
        m_resources[m_count] = resource;
        ++m_count;
    }
}

void ExportContext::releaseAll() {
    if (m_bridge) {
        for (std::size_t i = 0; i < m_count; ++i) {
            m_bridge->release(m_resources[i]);
        }
    }
    m_count = 0;
}

ApplyHandler* ExportContext::handlerAt(std::size_t index) const {
    if (index >= m_count) {
        return nullptr;
    }
    return static_cast<ApplyHandler*>(m_resources[index]);
}
