#ifndef _ALA_MAP_H
#define _ALA_MAP_H

#include <ala/detail/allocator.h>
#include <ala/detail/rb_tree.h>

namespace ala {

template<class Key, class T, class Comp = less<Key>,
         class Alloc = allocator<pair<const Key, T>>>
struct map: {
    typedef Key key_type;
    typedef T mapped_type;
    typedef pair<const Key, T> value_type;
    typedef ala::size_t size_type;
    typedef ala::ptrdiff_t difference_type;
    typedef Comp key_compare;
    typedef Alloc allocator_type;
    typedef value_type &reference;
    typedef const value_type &const_reference;
    typedef allocator_traits<Alloc>::pointer point;
    typedef allocator_traits<Alloc>::const_pointer const_point;
    typedef iterator;
    typedef const_iterator;
    typedef reverse_iterator;
    typedef const_reverse_iterator;
    typedef node_type;
    typedef insert_return_type;

    struct value_compare {
        friend class map<Key, T, Comp, Alloc>;

    protected:
        Comp comp;
        value_compare(Comp c): comp(c) {}

    public:
        bool operator()(const value_type &x, const value_type &y) const {
            return comp(x.first, y.first);
        }
    };

    key_compare key_comp() const { return key_compare(); }

    value_compare value_comp() const { return value_compare(key_comp()); }
}

} // namespace ala

#endif // HEAD