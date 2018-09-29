#ifndef DUFF_HPP
#define DUFF_HPP

#define DUFF_LOOP(ACTION) \
int duff_n = (Size + 7) / 8;\
switch (Size % 8) {\
	case 0: do { ACTION;\
	case 7: ACTION;\
	case 6: ACTION;\
	case 5: ACTION;\
	case 4: ACTION;\
	case 3: ACTION;\
	case 2: ACTION;\
	case 1: ACTION;\
	} while (--duff_n > 0);\
}

#endif // DUFF_HPP