#pragma once

#include <cstddef>

class ApplyHandler;
class LegacyBridge;

class ExportContext {
public:
    ExportContext() = default;
    explicit ExportContext(LegacyBridge* bridge);

    void setReleaser(LegacyBridge* bridge) { m_bridge = bridge; }
    void registerHandler(ApplyHandler* handler);
    void addResource(void* resource);
    void releaseAll();

    ApplyHandler* handlerAt(std::size_t index) const;
    std::size_t handlerCount() const { return m_count; }

private:
    LegacyBridge* m_bridge = nullptr;
    static constexpr std::size_t kMaxResources = 8;
    void* m_resources[kMaxResources];
    std::size_t m_count = 0;
};
