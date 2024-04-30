#pragma once

#include <new>

template<typename T>
class Allocator
{
public:
    T* allocate(const size_t n) {
        return static_cast<T*>(::operator new(sizeof(T) * n));
    }

    void deallocate(T* p, size_t n) {
        ::operator delete(p);
    }
};