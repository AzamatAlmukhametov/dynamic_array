#pragma once

template<typename T>
class Iterator
{
public:
    Iterator() : _p(nullptr) {}
    Iterator(T* const p) : _p(p) {}
    Iterator(const Iterator& it) : _p(it._p) {}
    operator Iterator<const T>() {
        return Iterator<const T>(this->_p);
    }

    Iterator& operator--() {
        --_p;
        return *this;
    }

    Iterator& operator++() {
        ++_p;
        return *this;
    }

    Iterator operator++(int) {
        Iterator prev(_p);
        ++_p;
        return prev;
    }

    Iterator operator--(int) {
        Iterator prev(_p);
        --_p;
        return prev;
    }

    T& operator*() {
        return *_p;
    }
    
    T* operator->() {
        return _p;
    }

    Iterator operator+(int64_t d) {
        return _p + d;
    }

    Iterator operator-(int64_t d) {
        return _p - d;
    }

    template<typename S>
    friend bool operator==(const Iterator<S>& lhs, const Iterator<S>& rhs);

    template<typename S1, typename S2>
    friend int64_t operator-(const Iterator<S1>& lhs, const Iterator<S2>& rhs);

private:
    T* _p;
};

template<typename S>
bool operator==(const Iterator<S>& lhs, const Iterator<S>& rhs) {
    return lhs._p == rhs._p;
}

template<typename S>
bool operator!=(const Iterator<S>& lhs, const Iterator<S>& rhs) {
    return !operator==(lhs, rhs);
}

template<typename S1, typename S2>
int64_t operator-(const Iterator<S1>& lhs, const Iterator<S2>& rhs) {
    return lhs._p - rhs._p;
}
