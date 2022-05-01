#ifndef CAPSTONE_HELPER_H
#define CAPSTONE_HELPER_H

#define ONCE(expr) static bool __once__ = [this]() { ((expr)); return true; }()

#endif //CAPSTONE_HELPER_H

/* Copyright © 2022 Aaron Alef */
