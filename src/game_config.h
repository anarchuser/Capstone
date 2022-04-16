#ifndef CAPSTONE_GAME_CONFIG_H
#define CAPSTONE_GAME_CONFIG_H

#define WINDOW_WIDTH    960
#define WINDOW_HEIGHT   640

#define SCALE           100.0f

#define FORCE           0.001
#define TORQUE          0.0001

#define DENSITY_PLANET  1000

#define BRAKE_TYPE_NONE       0     /// There are no brakes
#define BRAKE_TYPE_DIRECTED   1     /// Braking accelerates backwards
#define BRAKE_TYPE_UNDIRECTED 2     /// Braking universally decelerates
#define BRAKE_TYPE BRAKE_TYPE_UNDIRECTED

#define TORQUE_TYPE_CENTERED     0
#define TORQUE_TYPE_SIDE_IMPULSE 1
#define TORQUE_TYPE TORQUE_TYPE_SIDE_IMPULSE


#endif //CAPSTONE_GAME_CONFIG_H

/* Copyright © 2022 Aaron Alef */
