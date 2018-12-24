class C0 {
	C0() {}
};

class C1 {
	int v1;

  public:
	C1(int n) : v1(n) {}
};

class C2 {
	int v1;
	double v2;

  public:
	C2(int n) : v1(n), v2() {}
	C2(int n, double f)
	noexcept : v1(n), v2(f) {}
};

struct S0 {
};

struct S1 {
	int v1;
	S1(const S1& _s) : v1(_s.v1) {}
};

struct S2 {
	int v1;
	double v2;
	S2(S2&& _s) : v1(_s.v1),v2(_s.v2) {}
	S2(S0 s0):v1(0),v2(0) {}
};

enum E{};

union U{
	C0 c0;
	C1 c1;
};

using X0 =auto (int, int)->int (**)(int);
typedef int (*((*X1(int, int))))(int);
typedef char ** const * const X2;
typedef char (*(*X3[3])())[5];
typedef char (*(*X4())[5])();
typedef int (*(*X5)(const void *))[3];
typedef char (*(*X6())[5])();

struct S {
    double operator()(char, int&);
    float operator()(int) { return 1.0;}
};

template<class T>
typename ala::result_of<T(int)>::type f(T& t)
{
    std::cout << "overload of f for callable T\n";
    return t(0);
}

template<class T, class U>
int f(U u)
{
    std::cout << "overload of f for non-callable T\n";
    return u;
}

class CC {
	void pp() {}
	public:
	double v;
	int x(int y){
		return y;
	}
	float y(int x) {
		return x;
	}
	float z(float x) {
		return x;
	}
};


