#pragma once

#include <memory>
#include <stdexcept>

#include "Iterator.hpp"
#include "Allocator.hpp"

// insert, erase, emplace_back does not give a strong exception guarantee
// if move constructor or copy constructor throws

template<typename T, typename Allocator = Allocator<T>>
class DynamicArray
{
public:
    using value_type      = T;
    using reference       = T&;
    using const_reference = const T&;
    using pointer         = T*;
    using const_pointer   = const T*;
    using difference_type = ptrdiff_t;
    using size_type       = size_t;
    using iterator        = Iterator<T>;
    using const_iterator  = Iterator<const T>;
    using allocator       = Allocator;

    // Constructors, destructor, assignment
    DynamicArray() noexcept;
    explicit DynamicArray(const size_type size);
    DynamicArray(const DynamicArray& da);
    DynamicArray(DynamicArray&& da) noexcept;
    DynamicArray(const std::initializer_list<value_type>& l);
    ~DynamicArray();
    DynamicArray& operator=(const DynamicArray& da);
    DynamicArray& operator=(DynamicArray&& da) noexcept;

    // Modifiers
    void push_back(const value_type& val) { emplace_back(val); }
    void push_back(value_type&& val) { emplace_back(std::move(val)); }
    template<typename... Args> void emplace_back(Args&&... args);
    void pop_back() noexcept { std::destroy_at(_p + --_size); }
    iterator insert(const_iterator it, const value_type& val);
    iterator erase(const_iterator it);
    void resize(const size_type newSize);
    void reserve(const size_type size);
    void clear() noexcept;

    // Element access
    reference front() noexcept { return *_p; }
    reference back() noexcept { return *(_p + _size - 1); }
    reference operator[](const size_type key) noexcept { return *(_p + key); }
    const_reference operator[](const size_type key) const noexcept { return *(_p + key); }
    reference at(const size_type key);
    const_reference at(const size_type key) const { return at(key); }
    pointer data() noexcept { return _p; }
    const_pointer data() const noexcept { return _p; }

    // Info
    inline size_type size() const { return _size; }
    inline size_type capacity() const { return _capacity; }
    inline bool empty() const { return _size == 0; }

    // Iterators
    iterator begin() noexcept { return _p; }
    iterator end() noexcept { return _p + _size; }
    const_iterator cbegin() const noexcept { return _p; }
    const_iterator cend() const noexcept { return _p + _size; }

    // Non-member functions
    template<typename S, typename A>
    friend void swap(DynamicArray<S, A>& lhs, DynamicArray<S, A>& rhs) noexcept;

private:
    void increaseCapacity(const size_type delta);
    void decreaseCapacity(const size_type delta);
    iterator expandAndInsert(const_iterator it, const value_type& val);

    const size_type _deltaScale = 10;
    pointer _p;
    size_type _size;
    size_type _capacity;
    allocator alloc;
};

template<typename T, typename Allocator>
DynamicArray<T, Allocator>::DynamicArray() noexcept :
    _p(nullptr), _size(0), _capacity(0) {}

template<typename T, typename Allocator>
DynamicArray<T, Allocator>::DynamicArray(const size_type size) {
    _p = alloc.allocate(size);
    _capacity = size;
    _size = size;

    size_type i = 0;
    try {
        for (; i < _size; ++i) {
            std::construct_at(_p + i);
        }
    } catch (...) {
        for (size_type pi = 0; pi < i; ++pi) {
            std::destroy_at(_p + pi);
        }
        alloc.deallocate(_p, size);

        throw;
    }
}

template<typename T, typename Allocator>
DynamicArray<T, Allocator>::DynamicArray(const DynamicArray& da) {
    _p = alloc.allocate(da._capacity);
    _capacity = da._capacity;
    _size = da._size;

    size_type i = 0;
    try {
        for (; i < _size; ++i) {
            std::construct_at(_p + i, *(da._p + i));
        }
    } catch (...) {
        for (size_type pi = 0; pi < i; ++pi) {
            std::destroy_at(_p + pi);
        }
        alloc.deallocate(_p, da._capacity);

        throw;
    }
}

template<typename T, typename Allocator>
DynamicArray<T, Allocator>::DynamicArray(DynamicArray&& da) noexcept {
    _capacity = da._capacity;
    _size = da._size;
    _p = da._p;

    da._size = 0;
    da._capacity = 0;
    da._p = nullptr;
}

template<typename T, typename Allocator>
DynamicArray<T, Allocator>::DynamicArray(const std::initializer_list<T>& l) {
    _p = alloc.allocate(l.size());
    _capacity = l.size();
    _size = l.size();

    size_type i = 0;
    try {
        for (; i < _size; ++i) {
            std::construct_at(_p + i, *(l.begin() + i));
        }
    } catch (...) {
        for (size_type pi = 0; pi < i; ++pi) {
            std::destroy_at(_p + pi);
        }
        alloc.deallocate(_p, l.size());

        throw;
    }
}

template<typename T, typename Allocator>
DynamicArray<T, Allocator>::~DynamicArray() {
    for (size_type i = 0; i < _size; ++i) {
        std::destroy_at(_p + i);
    }
    alloc.deallocate(_p, _size);
}

template<typename T, typename Allocator>
DynamicArray<T, Allocator>&
DynamicArray<T, Allocator>::operator=(const DynamicArray<T, Allocator>& da) {
    if (this->_p == da._p) {
        return *this;
    }

    pointer p = alloc.allocate(da._capacity);

    size_type i = 0;
    try {
        for (; i < da._size; ++i) {
            std::construct_at(p + i, *(da._p + i));
        }
    } catch (...) {
        for (size_type pi = 0; pi < i; ++pi) {
            std::destroy_at(p + pi);
        }
        alloc.deallocate(p, da._capacity);

        throw;
    }

    for (size_type j = 0; j < _size; ++j) {
        std::destroy_at(_p + j);
    }
    alloc.deallocate(_p, _capacity);

    _p = p;
    _capacity = da._capacity;
    _size = da._size;

    return *this;
}

template<typename T, typename Allocator>
DynamicArray<T, Allocator>&
DynamicArray<T, Allocator>::operator=(DynamicArray<T, Allocator>&& da) noexcept {
    if (this->_p == da._p) {
        return *this;
    }

    for (size_type i = 0; i < _size; +i) {
        std::destroy_at(_p + i);
    }
    alloc.deallocate(_p, _capacity);

    _capacity = da._capacity;
    _size = da._size;
    _p = da._p;

    da._size = 0;
    da._capacity = 0;
    da._p = nullptr;

    return *this;
}

template<typename T, typename Allocator>
template<typename... Args>
void DynamicArray<T, Allocator>::emplace_back(Args&&... args) {
    if (_size == _capacity) {
        increaseCapacity(_deltaScale);
    }

    std::construct_at(_p + _size, std::forward<Args>(args)...);
    ++_size;
}

template<typename T, typename Allocator>
typename DynamicArray<T, Allocator>::iterator
DynamicArray<T, Allocator>::insert(const_iterator it, const value_type& val) {
    if (_capacity == _size) {
        return expandAndInsert(it, val);
    }

    value_type valCopy(val);

    size_type shift = static_cast<size_type>(end() - it);

    for (size_type i = 0; i < shift; ++i) {
        std::construct_at(_p + _size - i, std::move(*(_p + _size - 1 - i)));
        std::destroy_at(_p + _size - 1 - i);
    }

    std::construct_at(_p + _size - shift, std::move(valCopy));
    ++_size;

    return _p + _size - 1 - shift;
}

template<typename T, typename Allocator>
typename DynamicArray<T, Allocator>::iterator
DynamicArray<T, Allocator>::erase(const_iterator it) {
    size_type shift = static_cast<size_type>(it - begin());

    for (size_type i = shift; i + 1 < _size; ++i) {
        std::destroy_at(_p + i);
        std::construct_at(_p + i, std::move(*(_p + i + 1)));
    }
    std::destroy_at(_p + --_size);

    return _p + shift;
}

template<typename T, typename Allocator>
void DynamicArray<T, Allocator>::resize(const size_type newSize) {
    if (newSize <= _size) {
        decreaseCapacity(_capacity - newSize);
    } else {
        if (newSize > _capacity) {
            increaseCapacity(newSize - _capacity);
        } else {
            decreaseCapacity(_capacity - newSize);
        }

        size_type i = _size;
        try {
            for (; i < newSize; ++i) {
                std::construct_at(_p + i);
            }
        } catch (...) {
            for (size_type pi = _size; pi < i; ++pi) {
                std::destroy_at(_p + pi);
            }

            throw;
        }
        _size = newSize;
    }
}

template<typename T, typename Allocator>
void DynamicArray<T, Allocator>::reserve(const size_type size) {
    if (_capacity >= size) {
        return;
    }

    increaseCapacity(size - _capacity);
}

template<typename T, typename Allocator>
void DynamicArray<T, Allocator>::clear() noexcept {
    for (size_type i = 0; i < _size; ++i) {
        std::destroy_at(_p + i);
    }
    alloc.deallocate(_p, _size);

    _p = nullptr;
    _capacity = 0;
    _size = 0;
}

template<typename T, typename Allocator>
typename DynamicArray<T, Allocator>::reference
DynamicArray<T, Allocator>::at(const size_type key) {
    if (key >= _size) {
        throw std::out_of_range("index of element out of range");
    }

    return *(_p + key);
}

template<typename S, typename A>
bool operator==(const DynamicArray<S, A>& lhs,
                const DynamicArray<S, A>& rhs) noexcept {
    if (lhs.size() != rhs.size()) {
        return false;
    }

    using size_type = typename DynamicArray<S, A>::size_type;
    for (size_type i = 0; i < lhs.size(); ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }

    return true;
}

template<typename S, typename A>
std::weak_ordering
operator<=>(const DynamicArray<S, A>& lhs,
            const DynamicArray<S, A>& rhs) noexcept {
    using size_type = typename DynamicArray<S, A>::size_type;
    for (size_type i = 0; i < lhs.size() && i < rhs.size(); ++i) {
        if (lhs[i] < rhs[i]) {
            return std::weak_ordering::less;
        } else if (lhs[i] > rhs[i]) {
            return std::weak_ordering::greater;
        }
    }

    return lhs.size() <=> rhs.size();
}

template<typename S, typename A>
void swap(DynamicArray<S, A>& lhs,
          DynamicArray<S, A>& rhs) noexcept {
    std::swap(lhs._capacity, rhs._capacity);
    std::swap(lhs._size, rhs._size);
    std::swap(lhs._p, rhs._p);
}

template<typename T, typename Allocator>
void DynamicArray<T, Allocator>::increaseCapacity(const size_type delta) {
    pointer p = alloc.allocate(_capacity + delta);
    
    _capacity += delta;

    for (size_type i = 0; i < _size; ++i) {
        std::construct_at(p + i, std::move(*(_p + i)));
        std::destroy_at(_p + i);
    }

    alloc.deallocate(_p, _size);
    _p = p;
}

template<typename T, typename Allocator>
void DynamicArray<T, Allocator>::decreaseCapacity(const size_type delta) {
    pointer p = alloc.allocate(_capacity - delta);
    
    _capacity -= delta;

    for (size_type i = 0; i < _size && i < _capacity; ++i) {
        std::construct_at(p + i, std::move(*(_p + i)));
    }

    for (size_type i = 0; i < _size; ++i) {
        std::destroy_at(_p + i);
    }

    alloc.deallocate(_p, _size);
    _p = p;

    if (_capacity < _size) {
        _size = _capacity;
    }
}

template<typename T, typename Allocator>
typename DynamicArray<T, Allocator>::iterator
DynamicArray<T, Allocator>::expandAndInsert(const_iterator it, const value_type& val) {
    value_type valCopy(val);
    pointer p = alloc.allocate(_capacity + _deltaScale);

    size_type shift = static_cast<size_type>(it - begin());
    _capacity += _deltaScale;

    for (size_type i = 0; i < shift; ++i) {
        std::construct_at(p + i, std::move(*(_p + i)));
        std::destroy_at(_p + i);
    }

    std::construct_at(p + shift, std::move(valCopy));

    for (size_type i = shift; i < _size; ++i) {
        std::construct_at(p + i + 1, std::move(*(_p + i)));
        std::destroy_at(_p + i);
    }

    alloc.deallocate(_p, _size);
    _p = p;
    ++_size;

    return begin() + static_cast<int64_t>(shift);
}