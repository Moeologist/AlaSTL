template <typename...> struct _or_;
template <> struct _or_<> : false_type {};
template <typename B1> struct _or_<B1> : B1 {};
template <typename B1, typename B2> struct _or_<B1, B2> : conditional_t<B1::value, B1, B2> {};
template <typename B1, typename B2, typename B3, typename... Bn>
struct _or_<B1, B2, B3, Bn...> : conditional_t<B1::value, B1, _or_<B2, B3, Bn...>> {};

template <typename...> struct _and_;
template <> struct _and_<> : true_type {};
template <typename B1> struct _and_<B1> : B1 {};
template <typename B1, typename B2> struct _and_<B1, B2> : conditional_t<B1::value, B2, B1>::type {};
template <typename B1, typename B2, typename B3, typename... Bn>
struct _and_<B1, B2, B3, Bn...> : conditional_t<B1::value, _and_<B2, B3, Bn...>, B1> {};

template <typename B>
struct _not_ : bool_constant<!bool(B::value)> {};