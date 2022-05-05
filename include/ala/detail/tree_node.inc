#ifndef _ALA_DETAIL_TREE_NODE_INC
    #define _ALA_DETAIL_TREE_NODE_INC
#endif

#if !defined(_ALA_IS_MAP)
    #error Internal error, nerver use this head
#endif

#include <ala/detail/allocator.h>

#if _ALA_IS_MAP
    #define NODE _map_node_adaptor
#else
    #define NODE _set_node_adaptor
#endif

namespace ala {

template<class NodePtr, class Alloc>
struct NODE {
#if _ALA_IS_MAP
    typedef remove_const_t<decltype(declval<NodePtr>()->_data.first)> key_type;
    typedef decltype(declval<NodePtr>()->_data.second) mapped_type;
#else
    typedef decltype(declval<NodePtr>()->_data) value_type;
#endif
    typedef Alloc allocator_type;
    typedef typename pointer_traits<NodePtr>::element_type _ele_type;
    typedef allocator_traits<allocator_type> _alloc_traits;

protected:
    void possess(NODE &&other) noexcept {
        _ptr = ala::move(other._ptr);
        other._ptr = nullptr;
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_move_assignment::value>
    enable_if_t<Dummy> move_helper(NODE &&other) {
        _alloc() = ala::move(other._alloc());
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_move_assignment::value>
    enable_if_t<!Dummy> move_helper(NODE &&other) {
        assert(_alloc() == other._alloc());
    }

public:
    constexpr NODE() noexcept {}

    NODE(NODE &&other) {
        if (other) {
            this->possess(ala::move(other));
            this->alloc_ctor(ala::move(other._alloc()));
        }
    }

    NODE &operator=(NODE &&other) {
        clear();
        if (other) {
            this->possess(ala::move(other));
            this->move_helper(ala::move(other));
        }
        return *this;
    }

    ~NODE() {
        clear();
    }

    explicit operator bool() const noexcept {
        return !empty();
    }

    ALA_NODISCARD bool empty() const noexcept {
        return _ptr == nullptr;
    }

    allocator_type get_allocator() const {
        return _alloc();
    }

#if _ALA_IS_MAP
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

    template<bool Dummy = _alloc_traits::propagate_on_container_swap::value>
    enable_if_t<Dummy> swap_helper(NODE &other) noexcept {
        if (*this && other)
            ala::_swap_adl(_alloc(), other._alloc());
        else if (*this)
            other.alloc_ctor(ala::move(this->_alloc()));
        else if (other)
            this->alloc_ctor(ala::move(other._alloc()));
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_swap::value>
    enable_if_t<!Dummy>
    swap_helper(NODE &other) noexcept(_alloc_traits::is_always_equal::value) {
        assert(_alloc() == other._alloc());
    }

    void
    swap(NODE &other) noexcept(_alloc_traits::propagate_on_container_swap::value ||
                               _alloc_traits::is_always_equal::value) {
        this->swap_helper(other);
        ala::_swap_adl(_ptr, other._ptr);
    }

protected:
#if _ALA_IS_MAP
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
    node_pointer _ptr = nullptr;
    union {
        allocator_type _a;
        char placehold;
    };

    void clear() {
        if (_ptr != nullptr) {
            _alloc_traits::destroy(_alloc(), ala::addressof(_ptr->_data));
            _alloc_traits::template deallocate_object<remove_pointer_t<NodePtr>>(
                _alloc(), _ptr, 1);
        }
    }

    template<class... Args>
    void alloc_ctor(Args &&... args) {
        ::new (ala::addressof(_a)) allocator_type(ala::forward<Args>(args)...);
    }

    const allocator_type &_alloc() const {
        return _a;
    }

    allocator_type &_alloc() {
        return _a;
    }

    NODE(node_pointer p, const allocator_type &a): _ptr(p) {
        if (*this)
            this->alloc_ctor(a);
    }
};

template<class NodePtr, class Alloc>
void swap(NODE<NodePtr, Alloc> &lhs,
          NODE<NodePtr, Alloc> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

} // namespace ala

#undef NODE