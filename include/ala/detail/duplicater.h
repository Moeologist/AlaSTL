#ifndef _ALA_DETAIL_DUPLICATER_H
#define _ALA_DETAIL_DUPLICATER_H

template<class T>
struct _duplicater {
    const T &_ref;
    size_t _size;
    size_t _pos;
    _duplicater(size_t n, const T &v, size_t pos)
        : _size(n), _ref(v), _pos(pos) {}
    _duplicater(const _duplicater &) = default;
    _duplicater &operator++() {
        ++_pos;
        return *this;
    }
    _duplicater &operator--() {
        --_pos;
        return *this;
    }
    _duplicater operator++(int) {
        _duplicater tmp(*this);
        ++_pos;
        return tmp;
    }
    _duplicater operator--(int) {
        _duplicater tmp(*this);
        --_pos;
        return tmp;
    }
    const T &operator*() {
        return _ref;
    }
};

#endif