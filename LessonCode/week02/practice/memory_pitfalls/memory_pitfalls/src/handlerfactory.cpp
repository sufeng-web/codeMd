#include "handlerfactory.h"
#include "applyhandler.h"

ApplyHandler* HandlerFactory::createHandler(int mode) const {
    ApplyHandler* p = nullptr;
    switch (mode) {
    case 0:
        p = new FileApplyHandler();
        break;
    case 1:
        p = new ExportHandler();
        break;
    default:
        break;
    }
    return p;
}
