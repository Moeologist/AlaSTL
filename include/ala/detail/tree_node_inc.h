#ifndef _ALA_DETAIL_TREE_NODE_INC
#define _ALA_DETAIL_TREE_NODE_INC
#endif

#if !defined(IS_MAP)
#error "internal error, nerver use this head"
#endif

#include <ala/detail/allocator.h>

#if IS_MAP
#define NODE _map_node_adaptor
#else
#define NODE _set_node_adaptor
#endif

namespace ala {

template<class NodePtr, class Alloc>
struct NODE {
#if IS_MAP
    typedef remove_const_t<decltype(declval<NodePtr>()->_data.first)> key_type;
    typedef decltype(declval<NodePtr>()->_data.second) mapped_type;
#else
    typedef decltype(declval<NodePtr>()->_data) value_type;
#endif
    typedef Alloc allocator_type;
    typedef typename pointer_traits<NodePtr>::element_type _ele_type;
    typedef allocator_traits<allocator_type> _alloc_traits;
    typedef typename _alloc_traits::template rebind_alloc<_ele_type> _node_alloc;

    constexpr NODE() noexcept: _ptr(nullptr) {}

    NODE(NODE &&nh) {
        if (nh._ptr != nullptr) {
            _ptr = ala::move(nh._ptr);
            _a = ala::move(nh._a);
        }
        nh._ptr = nullptr;
    }

    NODE &operator=(NODE &&nh) {
        if (_ptr != nullptr) {
            _a.destroy(ala::addressof(_ptr->_data));
            _node_alloc().deallocate(_ptr, 1);
        }
        if (nh._ptr != nullptr) {
            _ptr = ala::move(nh._ptr);
            ALA_CONST_IF(
                _alloc_traits::propagate_on_container_move_assignment::value) {
                _a = ala::move(nh._ptr);
            }
        }
        nh._ptr = nullptr;
    }

    ~NODE() {
        if (_ptr != nullptr) {
            _a.destroy(ala::addressof(_ptr->_data));
            _node_alloc().deallocate(_ptr, 1);
        }
    }

    explicit operator bool() const noexcept {
        return !empty();
    }

    ALA_NODISCARD bool empty() const noexcept {
        return _ptr == nullptr;
    }

    allocator_type get_allocator() const {
        return _a;
    }

#if IS_MAP
    key_type &key() const {
        return const_cast<key_type &>(_ptr->_data.first);
    }

    mapped_type &mapped() const {
        return _ptr->_data.second;
    }
#else
    value_type &value() const {
        return _ptr->_data;
    }
#endif

    void swap(NODE &nh) noexcept(_alloc_traits::propagate_on_container_swap::value ||
                                 _alloc_traits::is_always_equal::value) {
        ala::swap(_ptr, nh._ptr);
        ALA_CONST_IF(_alloc_traits::propagate_on_container_swap::value) {
            ala::swap(_a, nh._a);
        }
    }

private:
#if IS_MAP
    template<class, class, class, class>
    friend class map;
    template<class, class, class, class>
    friend class multimap;
#else
    template<class, class, class>
    friend class set;
    template<class, class, class>
    friend class multiset;
#endif
    typedef NodePtr node_pointer;
    node_pointer _ptr;
    allocator_type _a;
    NODE(node_pointer p): _ptr(p), _a() {
        if (p->_is_nil)
            p = nullptr;
    }
};

template<class NodePtr, class Alloc>
void swap(NODE<NodePtr, Alloc> &lhs,
          NODE<NodePtr, Alloc> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

} // namespace ala

#undef NODE