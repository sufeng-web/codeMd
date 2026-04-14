#pragma once

class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void onUpdate() = 0;
};