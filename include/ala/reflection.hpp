#ifndef REFLECTION_DATA_HPP
#define REFLECTION_DATA_HPP

#include <string>
#include <map>

namespace reflector {

static std::map<std::string, void* (*)()> _reflection_map;
static void* creat_instance(std::string name) {
	if (_reflection_map[name]) {
		return reflector::_reflection_map[name]();
	}
	return nullptr;
}

#define REFLECTOR(name) \
static void* _reflection_constructor_##name() {\
	return new name();\
}\
struct _reflection_registor_##name {\
	_reflection_registor_##name() {\
		reflector::_reflection_map[#name] = _reflection_constructor_##name;\
	}\
};\
static _reflection_registor_##name _reflection_registor_##name##_instance;

} // reflector

#endif