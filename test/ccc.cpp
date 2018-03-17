// template <class T>
// struct reference_wrapper {
// 	typedef T type;
// 	reference_wrapper(T &ref) noexcept : _ptr(std::addressof(ref)) {}
// 	reference_wrapper(T &&) = delete;
// 	reference_wrapper(const reference_wrapper &) noexcept = default;
// 	reference_wrapper &operator=(const reference_wrapper &x) noexcept = default;
// 	operator T &() const noexcept { return *_ptr; }
// 	T &get() const noexcept { return *_ptr; }
// 	template <class... ArgTypes>
// 	invoke_result_t<T &, ArgTypes...>
// 	operator()(ArgTypes &&... args) const {
// 		return std::invoke(get(), forward<ArgTypes>(args)...);
// 	}

//   private:
// 	T *_ptr;
// };

// template <class T>
// reference_wrapper(reference_wrapper<T>)->reference_wrapper<T>;

// template <typename _Ret, typename _Func, typename... _Args>
// struct is_invocable_r : _is_invocable_impl<__invoke_result<_Func, _Args...>, _Ret> {};

#include <iostream>
#include <map>
#include <string>
using namespace std;

struct C{
	int _x;
	int _y;
	C(int x,int y=0): _x(x),_y(y) {}
	bool operator<(const C& c) const{
		return _x<c._x;
	}
};


int main()
{
	C c1(1),c2(2),c3(3);
	// c1==c2;
	map<C,string> m;
	m.insert(pair<C, string>(c1, "fuck"));
	// m.insert_or_assign(c1, "fucc");
	// m.insert_or_assign(c2,"shit");
	m.insert_or_assign(C(1,2),"shi");
	// m.insert_or_assign(c3,"shitt");
	// m.insert_or_assign(C(0),"s");
	// m.insert_or_assign(C(-1),"sx");
	cout<<m.size();
	
	auto x=m.begin();
	cout<<x->second;
	++x;
	cout<<sizeof(map<C,string>::iterator);
	return 0;
}