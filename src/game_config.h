#ifndef CAPSTONE_GAME_CONFIG_H
#define CAPSTONE_GAME_CONFIG_H

/// Width of application window
#define WINDOW_WIDTH    960
/// Height of application window
#define WINDOW_HEIGHT   640

/// Conversion rate between display (pixels) and physics
#define SCALE           100.0f

/// Force is applied forward each frame if accelerating
#define FORCE           0.002
/// TORQUE is the force each frame if rotating
#define TORQUE          0.0005

/// Density of planets dictates their gravitational pull
#define DENSITY_PLANET  1000
/// Planet mass multiplier to get final gravitational force
#define GRAVITY_PLANET  1e-6

/// If this is defined, use this as seed
//#define RNG_SEED        1

#define BRAKE_TYPE_NONE       0     /// There are no brakes
#define BRAKE_TYPE_DIRECTED   1     /// Braking accelerates backwards
#define BRAKE_TYPE_UNDIRECTED 2     /// Braking universally decelerates
#define BRAKE_TYPE BRAKE_TYPE_UNDIRECTED

#define TORQUE_TYPE_CENTERED     0  /// Rotation is induced using angular impulses
#define TORQUE_TYPE_SIDE_IMPULSE 1  /// Rotation uses rear corner thrusters
#define TORQUE_TYPE TORQUE_TYPE_SIDE_IMPULSE


#endif //CAPSTONE_GAME_CONFIG_H

/* Copyright © 2022 Aaron Alef */
