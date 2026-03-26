#include "legacybridge.h"
#include "guardalloc.h"
#include "ApplyHandler.h"
void LegacyBridge::genericFree(void* p) {
    if (p) {
        delete static_cast<ApplyHandler*>(p);
    }
}

void LegacyBridge::release(void* p) {

    genericFree(p);

}

void LegacyBridge::releaseLegacyInterface(void* interfacePtr) {
    if (!interfacePtr) {
        return;
    }
    delete static_cast<char*>(interfacePtr);
}

void LegacyBridge::submitString(const char* narrowStr) {
    if (!m_lastSubmitted && narrowStr) {
        m_lastSubmitted = narrowStr;
    }
    if (narrowStr) {
        validatePointer(narrowStr);
    }
    (void)narrowStr;
}

void LegacyBridge::auditLastSubmission() const {
    if (m_lastSubmitted) {
        validatePointer(m_lastSubmitted);
    }
}
