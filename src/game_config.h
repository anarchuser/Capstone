#ifndef CAPSTONE_GAME_CONFIG_H
#define CAPSTONE_GAME_CONFIG_H

/// Width of application window
#define WINDOW_WIDTH    1080
/// Height of application window
#define WINDOW_HEIGHT   1080

/// Conversion rate between display (pixels) and physics
#define WORLD_SCALE     100.0f

/// Spaceship scale
#define SPACESHIP_SCALE 0.2

/// Force is applied forward each frame if accelerating
#define FORCE           5e-4
/// TORQUE
#define TORQUE          5e-5

/// Density of planets dictates their gravitational pull
#define DENSITY_PLANET  1e3
// TODO: use reasonable physics calculation
/// Planet mass multiplier to get final gravitational force
#define GRAVITY_PLANET  2e-7

/// Spaceship density. Dictates inertia
#define DENSITY_SPACESHIP 2

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
