#pragma once

class LegacyBridge {
public:
    LegacyBridge() = default;

    void release(void* p);
    void releaseLegacyInterface(void* interfacePtr);
    void submitString(const char* narrowStr);

    // Used for audit trail; must be called after all submitString calls for current batch.
    void auditLastSubmission() const;

private:
    void genericFree(void* p);
    const char* m_lastSubmitted = nullptr;
};
