#pragma once

#include <cstddef>

// Places the buffer at the end of a page so the byte after the buffer is in a
// no-access page. A real out-of-bounds access then faults (Debug and Release)
// without any "crash for its own sake" code at the call site.

struct GuardedBlock {
    void* ptr = nullptr;
    void* base = nullptr;
};

// Allocates a block of `size` bytes with the next page set to no-access.
// One-past-end access will hit the guard page and crash (Debug + Release).
// Caller copies data from existing buffer, then frees the old buffer.
// Free with freeGuardedBlock(block).
GuardedBlock allocateWithTrailingGuard(const void* src, std::size_t size);

void freeGuardedBlock(GuardedBlock block);

// Validates that p is a valid pointer (one read at p; faults if null or no-access).
// Implemented in .cpp so the access is not optimized away in Release.
void validatePointer(const void* p);
