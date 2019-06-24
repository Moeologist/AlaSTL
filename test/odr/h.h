template<class>
struct Shit {
    static void bad() {}
};

template<>
inline void Shit<int>::bad() {}