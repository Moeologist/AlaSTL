template<typename _It>
struct iterator_traits {
    using value_type = typename _It::value_type;
};

template<typename _It>
struct iterator_traits<const _It> {
    using value_type = typename _It::value_type;
};

template<typename _It>
struct iterator_traits<_It *> {
    using value_type = _It;
};

template<typename _It>
struct iterator_traits<const _It *> {
    using value_type = _It;
};