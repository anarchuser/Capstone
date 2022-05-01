#ifndef CAPSTONE_DIRECTION_H
#define CAPSTONE_DIRECTION_H

struct Direction {
    char accelerate:  2 = false;
    char decelerate:  2 = false;
    char rotateLeft:  2 = false;
    char rotateRight: 2 = false;
};

#endif //CAPSTONE_DIRECTION_H

/* Copyright © 2022 Aaron Alef */
