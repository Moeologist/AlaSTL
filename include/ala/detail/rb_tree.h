// Introduction to Algorithms
#ifndef _ALA_DETAIL_RB_TREE_H
#define _ALA_DETAIL_RB_TREE_H

#include <ala/type_traits.h>
#include <ala/detail/allocator.h>
#include <ala/detail/pair.h>
#include <ala/detail/macro.h>
#include <ala/iterator.h>

#define ALA_RED true
#define ALA_BLACK false

#define ALA_RGHT true
#define ALA_LEFT false

namespace ala {

template<class Data>
struct rb_node {
    Data _data;
    rb_node *_left, *_rght, *_parent;
    bool _color, _is_nil, _nflag;
};

template<class Data>
constexpr bool is_black(rb_node<Data> *node) {
    return node == nullptr || !node->_color;
}

template<class Data>
constexpr bool is_red(rb_node<Data> *node) {
    return node != nullptr && node->_color;
}

template<class Data>
constexpr bool is_nil(rb_node<Data> *node) {
    return node == nullptr || node->_is_nil;
}

template<class Data>
constexpr rb_node<Data> *left_leaf(rb_node<Data> *node) {
    if (is_nil(node))
        return node;
    while (!is_nil(node->_left))
        node = node->_left;
    return node;
}

template<class Data>
constexpr rb_node<Data> *rght_leaf(rb_node<Data> *node) {
    if (is_nil(node))
        return node;
    while (!is_nil(node->_rght))
        node = node->_rght;
    return node;
}

template<class Data>
constexpr rb_node<Data> *next_node(rb_node<Data> *_ptr) {
    if (_ptr->_is_nil)
        return _ptr->_nflag ? _ptr : _ptr->_parent;
    else if (_ptr->_rght != nullptr)
        return left_leaf(_ptr->_rght);
    else {
        while (true) {
            if (_ptr->_parent->_left == _ptr) {
                _ptr = _ptr->_parent;
                break;
            }
            _ptr = _ptr->_parent;
        }
        return _ptr;
    }
}

template<class Data>
constexpr rb_node<Data> *prev_node(rb_node<Data> *_ptr) {
    if (_ptr->_is_nil)
        return !_ptr->_nflag ? _ptr : _ptr->_parent;
    else if (_ptr->_left != nullptr)
        return rght_leaf(_ptr->_left);
    else {
        while (true) {
            if (_ptr->_parent->_rght == _ptr) {
                _ptr = _ptr->_parent;
                break;
            }
            _ptr = _ptr->_parent;
        }
        return _ptr;
    }
}

template<class Value, class Ptr, bool IsConst = false>
struct rb_iterator {
    typedef bidirectional_iterator_tag iterator_category;
    typedef Value value_type;
    typedef typename pointer_traits<Ptr>::difference_type difference_type;
    typedef conditional_t<IsConst, const value_type, value_type> *pointer;
    typedef conditional_t<IsConst, const value_type, value_type> &reference;

    constexpr rb_iterator() {}
    constexpr rb_iterator(const rb_iterator &other): _ptr(other._ptr) {}
    constexpr rb_iterator(const rb_iterator<Value, Ptr, !IsConst> &other)
        : _ptr(other._ptr) {}
    constexpr rb_iterator(const Ptr &ptr): _ptr(ptr) {}

    constexpr reference operator*() const {
        return _ptr->_data;
    }

    constexpr pointer operator->() const {
        return ala::addressof(_ptr->_data);
    }

    constexpr bool operator==(const rb_iterator &rhs) const {
        return (_ptr == rhs._ptr);
    }

    constexpr bool operator!=(const rb_iterator &rhs) const {
        return !(_ptr == rhs._ptr);
    }

    constexpr bool operator==(const rb_iterator<Value, Ptr, !IsConst> &rhs) const {
        return (_ptr == rhs._ptr);
    }

    constexpr bool operator!=(const rb_iterator<Value, Ptr, !IsConst> &rhs) const {
        return !(_ptr == rhs._ptr);
    }

    constexpr rb_iterator &operator++() {
        _ptr = next_node(_ptr);
        return *this;
    }

    constexpr rb_iterator operator++(int) {
        rb_iterator tmp(*this);
        ++*this;
        return tmp;
    }

    constexpr rb_iterator &operator--() {
        _ptr = prev_node(_ptr);
        return *this;
    }

    constexpr rb_iterator operator--(int) {
        rb_iterator tmp(*this);
        --*this;
        return tmp;
    }

protected:
    friend class rb_iterator<Value, Ptr, !IsConst>;
    template<class, class, class, class>
    friend class map;
    template<class, class, class, class>
    friend class multimap;
    template<class, class, class>
    friend class set;
    template<class, class, class>
    friend class multiset;
    Ptr _ptr = nullptr;
};

template<class Data, class Comp, class Alloc, bool IsMap, bool IsUniq>
class rb_tree {
public:
    typedef Data value_type;
    typedef Comp value_compare;
    typedef Alloc allocator_type;
    typedef rb_node<Data> _node_t;
    typedef allocator_traits<allocator_type> _alloc_traits;
    typedef typename _alloc_traits::size_type size_type;
    typedef _node_t *_hdle_t;
    static constexpr bool is_uniq = IsUniq;

protected:
    template<class, class, class, bool, bool>
    friend class rb_tree;

    _hdle_t _root = nullptr;
    _hdle_t _guard = nullptr;

    size_type _size = 0;
    allocator_type _alloc;
    allocator_type _galloc = _alloc;
    value_compare _comp;

    _hdle_t left_nil() const noexcept {
        return _guard;
    }

    _hdle_t rght_nil() const noexcept {
        return _guard + 1;
    }

    template<typename T>
    struct _is_pair: false_type {};

    template<typename T1, typename T2>
    struct _is_pair<pair<T1, T2>>: true_type {
        using key = T1;
        using mapped = T2;
    };

    template<typename P, bool Dummy = IsMap, typename = enable_if_t<Dummy>>
    const auto &_key(const P &v) const noexcept {
        static_assert(_is_pair<P>::value == IsMap, "Internal error");
        return v.first;
    }

    template<typename P, bool Dummy = IsMap, typename = enable_if_t<!Dummy>>
    const value_type &_key(const P &v) const noexcept {
        // static_assert(_is_pair<P>::value == IsMap, "Internal error");
        return v;
    }

    template<bool Dummy = IsMap, typename = enable_if_t<Dummy>>
    decltype(auto) key_comp() const noexcept {
        return _comp.comp;
    }

    template<bool Dummy = IsMap, typename = enable_if_t<!Dummy>>
    const value_compare &key_comp() const noexcept {
        return _comp;
    }
    //TODO
    template<typename P, typename P1 = remove_cvref_t<P>,
             typename = enable_if_t<!is_lvalue_reference<P>::value>>
    auto pair_ref(P &&pr) const noexcept {
        using k = remove_const_t<typename P1::first_type>;
        using m = typename P1::second_type;
        return ala::pair<k &&, m &&>(ala::move(const_cast<k &>(pr.first)),
                                     ala::move(const_cast<m &>(pr.second)));
    }

    template<typename P, typename P1 = remove_cvref_t<P>,
             enable_if_t<is_lvalue_reference<P>::value, int> = 0>
    auto pair_ref(P &&pr) const noexcept {
        using k = remove_const_t<typename P1::first_type>;
        using m = typename P1::second_type;
        return ala::pair<k &, m &>(const_cast<k &>(pr.first),
                                   const_cast<m &>(pr.second));
    }

    template<class T1, class T2>
    bool kcmp(const T1 &lhs, const T2 &rhs) const {
        const auto &comp = key_comp();
        bool result = comp(lhs, rhs);
        if (result)
            assert(!comp(rhs, lhs));
        return result;
    }

    void clone(const rb_tree &other) {
        _root = copy_tree(other._root);
        _size = other._size;
        _comp = other._comp;
        fix_nil();
    }

    void clone(rb_tree &&other) {
        _root = copy_tree_mv(other._root);
        _size = other._size;
        _comp = other._comp;
        fix_nil();
        other.clear();
    }

    void possess(rb_tree &&other) noexcept {
        _root = other._root;
        _size = other._size;
        _comp = ala::move(other._comp);
        fix_nil();
        other._root = nullptr;
        other._size = 0;
        other.fix_nil();
    }

public:
    rb_tree(const value_compare &cmp, const allocator_type &a = allocator_type())
        : _comp(cmp), _alloc(a) {
        initialize();
    }

    rb_tree(const rb_tree &other)
        : _comp(other._comp),
          _alloc(_alloc_traits::select_on_container_copy_construction(
              other._alloc)) {
        initialize();
        this->clone(other);
    }

    rb_tree(rb_tree &&other)
        : _comp(ala::move(other._comp)), _alloc(ala::move(other._alloc)) {
        initialize();
        this->possess(ala::move(other));
    }

    rb_tree(const rb_tree &other, const allocator_type &a)
        : _comp(other._comp), _alloc(a) {
        initialize();
        this->clone(other);
    }

    rb_tree(rb_tree &&other, const allocator_type &a)
        : _comp(ala::move(other._comp)), _alloc(a) {
        initialize();
        if (_alloc == other._alloc)
            this->possess(ala::move(other));
        else
            this->clone(ala::move(other));
    }

    ~rb_tree() {
        clear();
        deinitialize();
    }

protected:
    template<bool Dummy = _alloc_traits::propagate_on_container_copy_assignment::value>
    enable_if_t<Dummy> copy_helper(const rb_tree &other) {
        if (_alloc != other._alloc)
            clear();
        _alloc = other._alloc;
        this->clone(other);
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_copy_assignment::value>
    enable_if_t<!Dummy> copy_helper(const rb_tree &other) {
        clear();
        this->clone(other);
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_move_assignment::value>
    enable_if_t<Dummy> move_helper(rb_tree &&other) noexcept {
        clear();
        _alloc = ala::move(other._alloc);
        this->possess(ala::move(other));
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_move_assignment::value>
    enable_if_t<!Dummy>
    move_helper(rb_tree &&other) noexcept(_alloc_traits::is_always_equal::value) {
        if (_alloc == other._alloc) {
            clear();
            this->possess(ala::move(other));
        } else {
            clear();
            this->clone(ala::move(other));
        }
    }

public:
    rb_tree &operator=(const rb_tree &other) {
        if (this != ala::addressof(other))
            copy_helper(other);
        return *this;
    }

    rb_tree &operator=(rb_tree &&other) noexcept(
        _alloc_traits::is_always_equal::value
            &&is_nothrow_move_assignable<value_compare>::value) {
        if (this != ala::addressof(other))
            move_helper(ala::move(other));
        return *this;
    }

    _hdle_t begin() const noexcept {
        return left_nil()->_parent;
    }

    _hdle_t end() const noexcept {
        return rght_nil();
    }

    void clear() {
        destruct_tree(_root);
        _root = nullptr;
        _size = 0;
        fix_nil();
    }

    size_type size() const noexcept {
        return _size;
    }

    void remove(_hdle_t position) {
        assert(!is_nil(position));
        destruct_node(detach(position));
    }

    _hdle_t extract(_hdle_t position) noexcept {
        if (is_nil(position))
            return nullptr;
        detach(position);
        position->_left = position->_rght = position->_parent = nullptr;
        return position;
    }

    allocator_type get_allocator() const noexcept {
        return _alloc;
    }

    value_compare value_comp() const noexcept {
        return _comp;
    }

    size_type max_size() const noexcept {
        return _alloc_traits::max_size(_alloc);
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_swap::value>
    enable_if_t<Dummy> swap_helper(rb_tree &other) {
        ala::swap(_alloc, other._alloc);
    }

    template<bool Dummy = _alloc_traits::propagate_on_container_swap::value>
    enable_if_t<!Dummy> swap_helper(rb_tree &other) {
        assert(_alloc == other._alloc);
    }

    void
    swap(rb_tree &other) noexcept(_alloc_traits::is_always_equal::value &&
                                      is_nothrow_swappable<value_compare>::value) {
        this->swap_helper(other);
        ala::swap(_comp, other._comp);
        ala::swap(_root, other._root);
        ala::swap(_size, other._size);
        this->fix_nil();
        other.fix_nil();
    }

    struct locate_states {
        _hdle_t postion;
        bool found;
        bool lr;
    };

    template<class RBTree>
    void merge(RBTree &src) {
        assert(_alloc == src._alloc);
        auto next = src.begin();
        for (auto i = src.begin(); i != src.end(); i = next) {
            next = next_node(i);
            locate_states ls = this->locate(nullptr, src._key(i->_data));
            if (!ls.found || !is_uniq)
                this->attach_to(ls, src.detach(i));
        }
    }

    template<class K>
    size_type erase(const K &k) {
        _hdle_t next = begin();
        size_type n = 0;
        for (_hdle_t i = begin(); i != end(); i = next) {
            next = next_node(i);
            if (!this->kcmp(k, this->_key(i->_data)) &&
                !this->kcmp(this->_key(i->_data), k)) {
                this->remove(i);
                ++n;
            }
        }
        return n;
    }

    template<class K, class F>
    auto traverse(const K &k, _hdle_t current, F f) const {
        while (!is_nil(current)) {
            const auto &ck = this->_key(current->_data);
            if (this->kcmp(k, ck))
                current = current->_left;
            else if (this->kcmp(ck, k))
                current = current->_rght;
            else
                return f(true, current);
        }
        return f(false, current);
    }

    template<class K>
    _hdle_t find_helper(const K &k, _hdle_t root) const {
        if (is_uniq)
            return this->traverse(k, root, [&](bool exist, _hdle_t cur) {
                return exist ? cur : end();
            });
        else
            return this->traverse(k, root, [&](bool exist, _hdle_t cur) {
                if (exist) {
                    _hdle_t l = this->find_helper(k, cur->_left);
                    if (l != end())
                        return l;
                    return cur;
                }
                return end();
            });
    }

    template<class K>
    _hdle_t find(const K &k) const {
        return this->find_helper(k, _root);
    }

    template<class K>
    bool contains(const K &k) const {
        return this->traverse(k, _root, [](bool exist, _hdle_t cur) {
            return exist ? true : false;
        });
    }

    template<class K>
    size_type count_helper(const K &k, _hdle_t root) const {
        return this->traverse(k, root, [&](bool exist, _hdle_t cur) {
            return exist ? 1 + this->count_helper(k, cur->_left) +
                               this->count_helper(k, cur->_rght) :
                           0;
        });
    }

    template<class K>
    size_type count(const K &k) const {
        return this->count_helper(k, _root);
    }

    pair<_hdle_t, bool> insert(_hdle_t hint, _hdle_t p) {
        if (is_nil(p))
            return pair<_hdle_t, bool>(end(), false);
        locate_states ls = locate(hint, this->_key(p->_data));
        if (is_uniq && ls.found) {
            return pair<_hdle_t, bool>(ls.postion, false);
        } else {
            attach_to(ls, p);
            return pair<_hdle_t, bool>(p, true);
        }
    }

    template<bool, class K, class M>
    struct _is_km_emp_helper: false_type {};

    template<class K, class M>
    struct _is_km_emp_helper<true, K, M>
        : _and_<is_same<remove_cvref_t<K>, remove_cvref_t<typename _is_pair<Data>::key>>> {
    };

    template<class... Ts>
    struct _is_km_emp: false_type {};

    template<class K, class M>
    struct _is_km_emp<K, M>: _is_km_emp_helper<IsMap, K, M> {};

    template<class... Ts>
    struct _is_v_emp: false_type {};

    template<class T>
    struct _is_v_emp<T>
        : _or_<is_same<remove_cvref_t<T>, value_type>,
               _and_<bool_constant<IsMap>, _is_pair<remove_cvref_t<T>>>> {};

    template<class K, class M>
    pair<_hdle_t, bool> insert_or_assign(_hdle_t hint, K &&k, M &&m) {
        static_assert(IsMap, "Internal error");
        locate_states ls = locate(hint, k);
        if (ls.found) {
            ls.postion->_data.second = ala::move(m);
            return pair<_hdle_t, bool>(ls.postion, false);
        } else {
            _hdle_t node = construct_node(ala::forward<K>(k), ala::forward<M>(m));
            attach_to(ls, node);
            return pair<_hdle_t, bool>(node, true);
        }
    }

    template<class... Args>
    enable_if_t<!_is_v_emp<Args...>::value && !_is_km_emp<Args...>::value,
                pair<_hdle_t, bool>>
    emplace(_hdle_t hint, Args &&... args) {
        _hdle_t node = construct_node(ala::forward<Args>(args)...);
        locate_states ls = locate(hint, this->_key(node->_data));
        if (is_uniq && ls.found) {
            destruct_node(node);
            return pair<_hdle_t, bool>(ls.postion, false);
        } else {
            attach_to(ls, node);
            return pair<_hdle_t, bool>(node, true);
        }
    }

    template<class V>
    enable_if_t<_is_v_emp<V>::value, pair<_hdle_t, bool>> emplace(_hdle_t hint,
                                                                  V &&v) {
        return this->emplace_v(hint, ala::forward<V>(v));
    }

    template<class K, class M>
    enable_if_t<_is_km_emp<K, M>::value, pair<_hdle_t, bool>>
    emplace(_hdle_t hint, K &&k, M &&m) {
        return this->emplace_k(k, hint, ala::forward<K>(k), ala::forward<M>(m));
    }

    template<class V>
    pair<_hdle_t, bool> emplace_v(_hdle_t hint, V &&v) {
        return this->emplace_k(this->_key(v), hint, ala::forward<V>(v));
    }

    template<class K, class... Args>
    pair<_hdle_t, bool> emplace_k(const K &k, _hdle_t hint, Args &&... args) {
        locate_states ls = locate(hint, k);
        if (is_uniq && ls.found) {
            return pair<_hdle_t, bool>(ls.postion, false);
        } else {
            _hdle_t node = construct_node(ala::forward<Args>(args)...);
            attach_to(ls, node);
            return pair<_hdle_t, bool>(node, true);
        }
    }

    template<class K>
    locate_states check_hint(_hdle_t hint, const K &k) {
        if (hint == nullptr || !is_nil(hint->_left) || size() == 0)
            return locate_states{nullptr, false, ALA_LEFT};
        _hdle_t prev = prev_node(hint);
        bool p = !is_nil(prev), h = !is_nil(hint);
        bool kh = false, hk = false, kp = false, pk = false;
        if (h) {
            kh = this->kcmp(k, this->_key(hint->_data));
            hk = this->kcmp(this->_key(hint->_data), k);
            if (!kh && !hk)
                return locate_states{hint, true, ALA_LEFT};
        }
        if (p) {
            pk = this->kcmp(this->_key(prev->_data), k);
            kp = this->kcmp(k, this->_key(prev->_data));
            if (h && pk && kh)
                return locate_states{hint, false, ALA_LEFT};
            else if (!h && !pk && !kp)
                return locate_states{prev, true, ALA_RGHT};
            else if (!h && !kp)
                return locate_states{prev, false, ALA_RGHT};
        } else if (kh)
            return locate_states{hint, false, ALA_LEFT};
        return locate_states{nullptr, false, ALA_LEFT};
    }

    template<class K>
    locate_states locate(_hdle_t hint, const K &k) {
        _hdle_t guard = _root, current = nullptr;
        bool found = false;
        bool lr = ALA_LEFT;
        locate_states checked = check_hint(hint, k);
        if (checked.postion != nullptr)
            return checked;
        while (!is_nil(guard)) {
            current = guard;
            const auto &ck = this->_key(current->_data);
            if (this->kcmp(k, ck)) {
                guard = guard->_left;
                lr = ALA_LEFT;
            } else if (this->kcmp(ck, k)) {
                guard = guard->_rght;
                lr = ALA_RGHT;
            } else {
                found = true;
                if (is_uniq)
                    break;
                lr = ALA_RGHT;
                guard = guard->_rght;
            }
        }
        return locate_states{current, found, lr};
    }

protected:
    template<class... Ts>
    struct _use_pair_ref: false_type {};

    template<class T>
    struct _use_pair_ref<T>
        : _and_<_is_pair<remove_cvref_t<T>>, bool_constant<IsMap>,
                _not_<is_constructible<value_type, T>>> {};

    template<class... Args>
    enable_if_t<sizeof...(Args) != 1 || !_use_pair_ref<Args...>::value, _hdle_t>
    construct_node(Args &&... args) {
        _hdle_t node = _alloc_traits::template allocate_object<_node_t>(_alloc,
                                                                        1);
        _alloc_traits::construct(_alloc, ala::addressof(node->_data),
                                 ala::forward<Args>(args)...);
        node->_is_nil = false;
        return node;
    }

    template<class P>
    enable_if_t<_use_pair_ref<P>::value, _hdle_t> construct_node(P &&pr) {
        _hdle_t node = _alloc_traits::template allocate_object<_node_t>(_alloc,
                                                                        1);
        _alloc_traits::construct(_alloc, ala::addressof(node->_data),
                                 this->pair_ref(ala::forward<P>(pr)));
        node->_is_nil = false;
        return node;
    }

    void destruct_node(_hdle_t node) {
        if (!node->_is_nil)
            _alloc_traits::destroy(_alloc, ala::addressof(node->_data));
        _alloc_traits::template deallocate_object<_node_t>(_alloc, node, 1);
        node = nullptr;
    }

    void destruct_tree(_hdle_t root) {
        if (is_nil(root))
            return;
        _hdle_t left = root->_left, rght = root->_rght;
        destruct_node(root);
        destruct_tree(left);
        destruct_tree(rght);
    }

    _hdle_t copy_tree(_hdle_t other, _hdle_t parent = nullptr) {
        if (is_nil(other))
            return nullptr;
        _hdle_t node = construct_node(other->_data);
        node->_color = other->_color;
        node->_parent = parent;
        node->_left = copy_tree(other->_left, node);
        node->_rght = copy_tree(other->_rght, node);
        return node;
    }

    _hdle_t copy_tree_mv(_hdle_t other, _hdle_t parent = nullptr) {
        if (is_nil(other))
            return nullptr;
        _hdle_t node = construct_node(ala::move(other->_data));
        node->_color = other->_color;
        node->_parent = parent;
        node->_left = copy_tree_mv(other->_left, node);
        node->_rght = copy_tree_mv(other->_rght, node);
        return node;
    }

    void initialize() {
        _guard = _alloc_traits::template allocate_object<_node_t>(_galloc, 2);
        left_nil()->_is_nil = true;
        left_nil()->_parent = rght_nil();
        left_nil()->_color = ALA_BLACK;
        left_nil()->_nflag = ALA_LEFT;
        left_nil()->_left = nullptr;
        left_nil()->_rght = nullptr;
        rght_nil()->_is_nil = true;
        rght_nil()->_parent = left_nil();
        rght_nil()->_color = ALA_BLACK;
        rght_nil()->_nflag = ALA_RGHT;
        rght_nil()->_left = nullptr;
        rght_nil()->_rght = nullptr;
    }

    void deinitialize() {
        _alloc_traits::template deallocate_object<_node_t>(_galloc, left_nil(),
                                                           2);
    }

    // rotate
    /*-------------------------------------
    |      x        left        y         |
    |     / \       ====>      / \        |
    |    a   y                x   c       |
    |       / \     <====    / \          |
    |      b   c    right   a   b         |
    --------------------------------------*/

    _hdle_t rotate_left(_hdle_t x) noexcept {
        _hdle_t y = x->_rght;
        if ((x->_rght = y->_left) != nullptr)
            // b can't be equal to NIL,so using is_nil is correct,too
            y->_left->_parent = x;
        y->_left = x;
        if ((y->_parent = x->_parent) == nullptr)
            _root = y;
        else if (x == x->_parent->_left)
            x->_parent->_left = y;
        else
            x->_parent->_rght = y;
        x->_parent = y;
        return y;
    }

    _hdle_t rotate_rght(_hdle_t y) noexcept {
        _hdle_t x = y->_left;
        if ((y->_left = x->_rght) != nullptr)
            x->_rght->_parent = y;
        x->_rght = y;
        if ((x->_parent = y->_parent) == nullptr)
            _root = x;
        else if (y == y->_parent->_left)
            y->_parent->_left = x;
        else
            y->_parent->_rght = x;
        y->_parent = x;
        return x;
    }

    /*---------------------------------
    |      up     trans     up        |
    |       \     ====>      \        |
    |        u                v       |
    ---------------------------------*/

    void transplant(_hdle_t u, _hdle_t v) noexcept {
        _hdle_t uparent = u->_parent;
        if (uparent == nullptr)
            _root = v;
        else if (u == uparent->_left)
            uparent->_left = v;
        else
            uparent->_rght = v;
        if (v != nullptr)
            v->_parent = uparent;
    }

    void fix_nil() noexcept {
        if (_root == nullptr) {
            left_nil()->_parent = rght_nil();
            rght_nil()->_parent = left_nil();
        } else {
            _hdle_t lleaf = left_leaf(_root);
            _hdle_t rleaf = rght_leaf(_root);
            lleaf->_left = left_nil();
            rleaf->_rght = rght_nil();
            left_nil()->_parent = lleaf;
            rght_nil()->_parent = rleaf;
        }
    }

    void attach_to(locate_states states, _hdle_t p) noexcept {
        _hdle_t pos = states.postion;
        p->_color = ALA_RED;
        p->_left = p->_rght = nullptr;
        p->_parent = pos;
        if (pos == nullptr) { // empty tree
            _root = p;
            left_nil()->_parent = rght_nil()->_parent = p;
            p->_left = left_nil();
            p->_rght = rght_nil();
        } else if (states.lr == ALA_LEFT) {
            if (pos->_left == left_nil()) { // fix nil
                p->_left = left_nil();
                left_nil()->_parent = p;
            }
            pos->_left = p;
        } else {
            if (pos->_rght == rght_nil()) { // fix nil
                p->_rght = rght_nil();
                rght_nil()->_parent = p;
            }
            pos->_rght = p;
        }
        ++_size;
        rebalance_for_attach(p);
    }

    /*---------------------------------------------------------------------------------
    |      Gb       P.c=b        Gb                    Gb                    Pb       |
    |     / \       U.c=b       / \       RotL(P)     / \       RotR(G)     / \       |
    |   Pr   Ur     ====>*    Pr   Ub     ====>     Pr   Ub     ====>     Xr   Gr     |
    |  / \          G.c=r    / \          P<->X    / \          P.c=b         / \     |
    |     Xr        X<-G        Xr                Xr            G.c=r            Ub   |
    ---------------------------------------------------------------------------------*/
    // remove continuous red
    // X: current, P: parent, U:uncle, G:grandp
    // b/r: black/red
    // c: color, L: left, R: right
    // <-: assign, <->: swap
    // =>: jump to next case, *: jump to a possiable case
    void rebalance_for_attach(_hdle_t current) noexcept {
        _hdle_t parent, grandp, uncle;
        while (is_red(parent = current->_parent)) {
            grandp = parent->_parent;
            if (parent == grandp->_left) { // see graph
                uncle = grandp->_rght;
                if (is_red(uncle)) {
                    parent->_color = uncle->_color = ALA_BLACK;
                    grandp->_color = ALA_RED;
                    current = grandp;
                } else {
                    if (parent->_rght == current)
                        parent = rotate_left(current = parent);
                    parent->_color = ALA_BLACK;
                    grandp->_color = ALA_RED;
                    rotate_rght(grandp);
                }
            } else { // symmetrical
                uncle = grandp->_left;
                if (is_red(uncle)) {
                    parent->_color = uncle->_color = ALA_BLACK;
                    grandp->_color = ALA_RED;
                    current = grandp;
                } else {
                    if (parent->_left == current)
                        parent = rotate_rght(current = parent);
                    parent->_color = ALA_BLACK;
                    grandp->_color = ALA_RED;
                    rotate_left(grandp);
                }
            }
        }
        _root->_color = ALA_BLACK;
    }

    _hdle_t detach(_hdle_t current) noexcept {
        _hdle_t child, parent, subs, fix = nullptr;
        bool color;
        if (is_nil(current->_left)) {
            color = current->_color;
            child = current->_rght;
            parent = current->_parent;
            transplant(current, child);
            fix = child == nullptr ? parent : child;
        } else if (is_nil(current->_rght)) {
            color = current->_color;
            child = current->_left;
            parent = current->_parent;
            transplant(current, child);
            fix = child == nullptr ? parent : child;
        } else {
            subs = left_leaf(current->_rght);
            color = subs->_color;
            child = subs->_rght;
            parent = subs->_parent;
            if (parent == current) {
                parent = subs;
            } else {
                transplant(subs, child);
                subs->_rght = current->_rght;
                subs->_rght->_parent = subs;
            }
            transplant(current, subs);
            subs->_left = current->_left;
            subs->_left->_parent = subs;
            subs->_color = current->_color;
            fix = subs;
        }
        --_size;
        if (_root != nullptr && color == ALA_BLACK)
            rebalance_for_detach(child, parent);
        if (fix)
            fix_nil_detach(current, fix);
        return current;
    }

    /*----------------------------------------------------------------------------------------------------
    |      Pb       P.c=r        P?                    P?       Y.L.c=b      P?       Y.c=P.c      Y?    |
    |     / \       Y.c=b       / \       Y.c=R       / \       Y.c=r       / \       P.c=b       / \    |
    |   Xb   Yr     ====>*    Xb   Yb     ====>*    Xb   Yb     ====>     Xb   Yb     ====>     Pb   b   |
    |       / \     RotL(P)       / \     X<-P          / \     RotR(Y)       / \     Y.R.c=b  / \       |
    |     P.R       Y=P.R        b   b                 r   b    Y=P.R            r    RotL(Y) X          |
    ----------------------------------------------------------------------------------------------------*/
    // make current black-height + 1
    // Y: brother

    void rebalance_for_detach(_hdle_t current, _hdle_t parent) noexcept {
        _hdle_t brother;
        while (current != _root && is_black(current)) {
            if (parent->_left == current) {
                brother = parent->_rght;
                assert(!is_nil(brother));
                if (brother->_color == ALA_RED) {
                    brother->_color = ALA_BLACK;
                    parent->_color = ALA_RED;
                    rotate_left(parent);
                    brother = parent->_rght;
                }
                if (is_black(brother->_left) && is_black(brother->_rght)) {
                    brother->_color = ALA_RED;
                    current = parent;
                    parent = current->_parent;
                } else {
                    if (is_black(brother->_rght)) {
                        if (!is_nil(brother->_left))
                            brother->_left->_color = ALA_BLACK;
                        brother->_color = ALA_RED;
                        rotate_rght(brother);
                        brother = parent->_rght;
                    }
                    brother->_color = parent->_color;
                    parent->_color = ALA_BLACK;
                    if (!is_nil(brother->_rght))
                        brother->_rght->_color = ALA_BLACK;
                    rotate_left(parent);
                    current = _root; // make root black
                    break;
                }
            } else {
                brother = parent->_left;
                if (brother->_color == ALA_RED) {
                    brother->_color = ALA_BLACK;
                    parent->_color = ALA_RED;
                    rotate_rght(parent);
                    brother = parent->_left;
                }
                if (is_black(brother->_left) && is_black(brother->_rght)) {
                    brother->_color = ALA_RED;
                    current = parent;
                    parent = current->_parent;
                } else {
                    if (is_black(brother->_left)) {
                        if (!is_nil(brother->_rght))
                            brother->_rght->_color = ALA_BLACK;
                        brother->_color = ALA_RED;
                        rotate_left(brother);
                        brother = parent->_left;
                    }
                    brother->_color = parent->_color;
                    parent->_color = ALA_BLACK;
                    if (!is_nil(brother->_left))
                        brother->_left->_color = ALA_BLACK;
                    rotate_rght(parent);
                    current = _root;
                    break;
                }
            }
        }
        if (!is_nil(current))
            current->_color = ALA_BLACK;
    }

    void fix_nil_detach(_hdle_t current, _hdle_t subs) noexcept {
        if (current->_left == left_nil()) {
            if (current->_rght == rght_nil()) {
                left_nil()->_parent = rght_nil();
                rght_nil()->_parent = left_nil();
                _root = nullptr;
            } else {
                subs->_left = left_nil();
                left_nil()->_parent = subs;
            }
        } else if (current->_rght == rght_nil()) {
            subs->_rght = rght_nil();
            rght_nil()->_parent = subs;
        }
    }
}; // namespace ala

} // namespace ala

#undef ALA_RED
#undef ALA_BLACK
#undef ALA_RGHT
#undef ALA_LEFT

#endif // HEAD