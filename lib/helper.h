#ifndef CAPSTONE_HELPER_H
#define CAPSTONE_HELPER_H

#include <iostream>

/// If this is defined, backend debug output will be more fine-grained
#define DEBUG_MINOR

/// Output function printing to stdout if debug mode is enabled
inline void debug_stdout (std::string const & message) {
#ifdef DEBUG
    std::cout << message << std::endl;
#endif
}

/// Generic macro for converting a member function into a lambda. Use with caution
#define LAMBDA(func) [this] (auto ... args) { return func (args ...); }

#endif //CAPSTONE_HELPER_H

/* Copyright © 2022 Aaron Alef */
