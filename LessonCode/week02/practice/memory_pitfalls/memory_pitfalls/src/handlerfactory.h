#pragma once

class ApplyHandler;

class HandlerFactory {
public:
    HandlerFactory() = default;

    ApplyHandler* createHandler(int mode) const;
    void setDefaultMode(int mode) { m_defaultMode = mode; }

private:
    int m_defaultMode = 0;
};
