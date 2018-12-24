#ifndef _ALA_ANY_H
#define _ALA_ANY_H

namespace ala {

#if _ALA_ENABLE_EXCEPTIONS
struct bad_any_cast : std::bad_cast {
	virtual const char *what() const noexcept {
		return "bad_any_cast";
	}
};
#endif

[[noreturn]] _ALA_FORCEINLINE void __throw_bad_any_cast() {
#if _ALA_ENABLE_EXCEPTIONS
	throw bad_any_cast();
#else
	std::abort();
#endif
}

struct any {
	struct placeholder {
		virtual ~placeholder() {}
		virtual placeholder *clone() const = 0;
	};

	template <typename T>
	struct holder : placeholder {
		typedef T value_type;
		T held;
		holder(const T &value) : held(value) {}
		virtual placeholder *clone() const {
			return new holder(held);
		}
	};

	placeholder *content;

  public:
	constexpr any() noexcept : content(nullptr) {}
	any(const any &other) : content(nullptr) {
		if (other.content) {
		}
	}
	any(const T &value) : content(new holder<T>(value)) {}

	template <typename T>
	any &operator=(const T &rhs) {
		delete content;
		content = new holder<T>(rhs);
		return *this;
	}

	~any() {
		delete content;
	}
};
} // namespace ala

#endif