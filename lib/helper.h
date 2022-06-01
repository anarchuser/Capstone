#ifndef CAPSTONE_HELPER_H
#define CAPSTONE_HELPER_H

#include <iostream>

#define DEBUG_MINOR

inline void debug_stdout (std::string const & message) {
#ifdef DEBUG
    std::cout << message << std::endl;
#endif
}

#define ONCE(expr) static bool __once__ = [this]() { ((expr)); return true; }()

#define LAMBDA(func) [this] (auto ...args) { return func (args...); }

#endif //CAPSTONE_HELPER_H

/* Copyright © 2022 Aaron Alef */
