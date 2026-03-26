#include "guardalloc.h"
#include <cstring>

#ifdef _WIN32
#include <Windows.h>

static std::size_t getPageSize() {
    SYSTEM_INFO si = {};
    GetSystemInfo(&si);
    return si.dwPageSize;
}

GuardedBlock allocateWithTrailingGuard(const void* src, std::size_t size) {
    GuardedBlock out = {};
    if (!src || size == 0) {
        return out;
    }
    const std::size_t pageSize = getPageSize();
    if (size > pageSize) {
        return out;
    }
    void* base = VirtualAlloc(nullptr, 2 * pageSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!base) {
        return out;
    }
    std::size_t offset = pageSize - size;
    void* ptr = static_cast<char*>(base) + offset;
    std::memcpy(ptr, src, size);
    DWORD old = 0;
    if (!VirtualProtect(static_cast<char*>(base) + pageSize, pageSize, PAGE_NOACCESS, &old)) {
        VirtualFree(base, 0, MEM_RELEASE);
        return out;
    }
    out.ptr = ptr;
    out.base = base;
    return out;
}

void freeGuardedBlock(GuardedBlock block) {
    if (block.base) {
        VirtualFree(block.base, 0, MEM_RELEASE);
    }
}

#else

GuardedBlock allocateWithTrailingGuard(const void* src, std::size_t size) {
    (void)src;
    (void)size;
    return {};
}

void freeGuardedBlock(GuardedBlock block) {
    (void)block;
}

#endif

void validatePointer(const void* p) {
    //------fix--------
    if (p == nullptr) {
        // 处理空指针，例如记录日志、抛出异常或直接返回
        return;
    }
    (void)*static_cast<volatile const char*>(p);
}
