#pragma once

#ifdef __GNUC__
#include <cxxabi.h>
#endif

#include <string>
#include <util/types.h>

namespace calamus {

/**
 * Because type_info::name() is implementation defined and most compilers return unusable
 * mangled garbage.
 */
template <typename T>
inline std::string type_name() {
    i32 status = 0;
    auto name = std::string { typeid(T).name() };
    // MSVC should already return a demangled name
#ifdef __GNUC__
    // TODO Test clang
    auto* demangled = abi::__cxa_demangle(name.c_str(), nullptr, nullptr, &status);
    if (status == 0) {
        // __cxa_demangle allocates, so copy its return value into a string before freeing.
        name = std::string { demangled };
        free(demangled);
    }
#endif
    return name;
}

}
