#define REPEAT_2(OPERATOR) \
x OPERATOR \
y OPERATOR

#define REPEAT_3(OPERATOR) \
x OPERATOR \
y OPERATOR \
z OPERATOR

#define REPEAT_4(OPERATOR) \
x OPERATOR \
y OPERATOR \
z OPERATOR \
w OPERATOR

#define REPEAT_5(OPERATOR) \
_m0 OPERATOR \
_m1 OPERATOR \
_m2 OPERATOR \
_m3 OPERATOR \
_m4 OPERATOR

#define REPEAT_6(OPERATOR) \
_m0 OPERATOR \
_m1 OPERATOR \
_m2 OPERATOR \
_m3 OPERATOR \
_m4 OPERATOR \
_m5 OPERATOR

#define REPEAT_7(OPERATOR) \
_m0 OPERATOR \
_m1 OPERATOR \
_m2 OPERATOR \
_m3 OPERATOR \
_m4 OPERATOR \
_m5 OPERATOR \
_m6 OPERATOR

#define REPEAT_8(OPERATOR) \
_m0 OPERATOR \
_m1 OPERATOR \
_m2 OPERATOR \
_m3 OPERATOR \
_m4 OPERATOR \
_m5 OPERATOR \
_m6 OPERATOR \
_m7 OPERATOR

#define REPEAT_2(OPERATOR, OPERATOR1) \
OPERATOR \
OPERATOR

#define REPEAT_3(OPERATOR, OPERATOR1) \
OPERATOR \
OPERATOR \
OPERATOR

#define REPEAT_4(OPERATOR, OPERATOR1) \
OPERATOR \
OPERATOR \
OPERATOR \
OPERATOR

#define REPEAT_5(OPERATOR, OPERATOR1) \
OPERATOR \
OPERATOR \
OPERATOR \
OPERATOR \
OPERATOR

#define REPEAT_6(OPERATOR, OPERATOR1) \
OPERATOR \
OPERATOR \
OPERATOR \
OPERATOR \
OPERATOR \
OPERATOR

#define REPEAT_7(OPERATOR, OPERATOR1) \
OPERATOR \
OPERATOR \
OPERATOR \
OPERATOR \
OPERATOR \
OPERATOR \
OPERATOR

#define REPEAT_8(OPERATOR, OPERATOR1) \
OPERATOR \
OPERATOR \
OPERATOR \
OPERATOR \
OPERATOR \
OPERATOR \
OPERATOR \
OPERATOR

#define MEM_2 \
T x, y;

#define MEM_3 \
union {\
	struct {\
		T x, y, z;\
	};\
	struct {\
		T r, g, b;\
	};\
};\

#define MEM_4 \
union {\
	struct {\
		T x, y, z, w;\
	};\
	struct {\
		T r, g, b, a;\
	};\
};\

#define MEM_5 _m0, _m1, _m2, _m3, _m4
#define MEM_6 _m0, _m1, _m2, _m3, _m4, _m5
#define MEM_7 _m0, _m1, _m2, _m3, _m4, _m5, _m6
#define MEM_8 _m0, _m1, _m2, _m3, _m4, _m5, _m6, _m7