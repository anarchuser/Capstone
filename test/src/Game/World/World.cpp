#include <catch2/catch_test_macros.hpp>

#include "Algebra/algebra.h"

#include "World/World.h"
#include "Planet/Planet.h"
#include "Random/random.h"

#define WINDOW_SIZE 100
#define PHYS_SCALE  0.1

#define TIME_STEPS 61

#define TEST_RANDOM_SEED 1

SCENARIO ("World handles objects as expected") {
    auto rng = HashedRNG (TEST_RANDOM_SEED);

    GIVEN ("An empty world") {
        Stage::instance = new Stage;
        getStage()->setSize (WINDOW_SIZE, WINDOW_SIZE);

        auto world = kt::World (nullptr, {WINDOW_SIZE * PHYS_SCALE, WINDOW_SIZE * PHYS_SCALE});

        THEN ("The physical world starts as default") {
            auto & physical_world = world.world;

            REQUIRE (physical_world.GetBodyCount() == 0);
            REQUIRE (physical_world.GetGravity() == b2Vec2_zero);
        }

        WHEN ("physics and game sizes are used") {
            auto phy_size = world.world_size;
            auto win_size = world.getSize();

            THEN ("They can be converted back and forth") {
                REQUIRE (win_size == getStage ()->getSize ());
                REQUIRE (world.convert (win_size) == phy_size);
                REQUIRE (world.convert (phy_size) == win_size);
                REQUIRE (world.convert (world.convert (win_size)) == win_size);
                REQUIRE (world.convert (world.convert (phy_size)) == phy_size);
            }
        }

        WHEN ("Objects are added within range") {
            auto pos = Vector2 (25, 50);
            auto phys_pos = b2Vec2 (PHYS_SCALE * 25, PHYS_SCALE * 50);
            kt::spPlanet planet = new kt::Planet (world, nullptr, pos);
            world.addChild (planet);

            THEN ("They are appropriately scaled in the world") {
                auto & child = * world.getLastChild ();
                REQUIRE (child.getPosition () == pos);
                REQUIRE (world.convert (child.getPosition ()) == phys_pos);

                auto * body = (b2Body *) child.getUserData ();
                REQUIRE (body->GetPosition () == phys_pos);
                REQUIRE (world.convert (body->GetPosition ()) == pos);
            }
        }

        WHEN ("Objects are randomly added within range") {
            auto pos = Vector2 (rng.random ({0, WINDOW_SIZE}), rng.random ({0, WINDOW_SIZE}));
            auto phys_pos = b2Vec2 (pos.x * PHYS_SCALE, pos.y * PHYS_SCALE);
            kt::spPlanet planet = new kt::Planet (world, nullptr, pos);
            world.addChild (planet);

            THEN ("They are appropriately scaled in the world") {
                auto & child = * world.getLastChild ();

                REQUIRE (child.getPosition () == pos);
                REQUIRE (world.convert (child.getPosition ()) == phys_pos);

                auto * body = (b2Body *) child.getUserData ();
                REQUIRE (body->GetPosition() == phys_pos);
                REQUIRE (world.convert (body->GetPosition ()) == pos);
            }
        }

        WHEN ("Objects are added outside range") {
            auto pos = Vector2 (-200, 500);
            auto phys_pos = b2Vec2 (PHYS_SCALE * -200, PHYS_SCALE * 500);

            REQUIRE (world.convert (pos) == phys_pos);
            REQUIRE (world.convert (phys_pos) == pos);

            kt::spPlanet planet = new kt::Planet (world, nullptr, pos);
            world.addChild (planet);

            THEN ("They are appropriately scaled in the world") {
                auto & child = * world.getLastChild ();
                REQUIRE (child.getPosition () != pos);
                REQUIRE (world.convert (child.getPosition ()) != phys_pos);
                REQUIRE (world.convert (child.getPosition ()) == world.wrap (phys_pos));

                auto * body = (b2Body *) child.getUserData ();
                REQUIRE (body->GetPosition () != phys_pos);
                REQUIRE (world.convert (body->GetPosition ()) != pos);
                REQUIRE (body->GetPosition () == world.wrap (phys_pos));
            }
        }

        WHEN ("Objects are randomly added outside range") {
            auto pos = Vector2 (rng.random ({-1e8, 0}), rng.random ({WINDOW_SIZE, 1e8}));
            auto phys_pos = b2Vec2 (pos.x * PHYS_SCALE, pos.y * PHYS_SCALE);

            REQUIRE (world.convert (pos) == phys_pos);
            REQUIRE (world.convert (phys_pos) == pos);

            kt::spPlanet planet = new kt::Planet (world, nullptr, pos);
            world.addChild (planet);

            THEN ("They are appropriately scaled in the world") {
                auto & child = * world.getLastChild ();
                REQUIRE (child.getPosition () != pos);
                REQUIRE (world.convert (child.getPosition ()) != phys_pos);
                REQUIRE (world.convert (child.getPosition ()) == world.wrap (phys_pos));

                auto * body = (b2Body *) child.getUserData ();
                REQUIRE (body->GetPosition () != phys_pos);
                REQUIRE (world.convert (body->GetPosition ()) != pos);
                REQUIRE (body->GetPosition () == world.wrap (phys_pos));
            }
        }
    }
}

SCENARIO ("Time passes in a near-empty world") {
    GIVEN ("A world containing one spaceship") {
        b2Vec2 world_size {10, 10};
        auto world = kt::World (nullptr, world_size);

        auto ship_pos = 0.5 * world_size;
        auto & ship = * new kt::Spaceship (world, nullptr, world.convert (ship_pos), 1);
        auto ship_ref = safeSpCast <kt::Spaceship> (world.getFirstChild());
        auto ship_angle = ship.getRotation();

        REQUIRE (& ship == ship_ref);
        REQUIRE (ship.getPhysicalPosition() == ship_pos);
        REQUIRE (ship.getPhysicalVelocity() == b2Vec2_zero);

        for (int i = 0; i < TIME_STEPS; i++) {
            WHEN ("Time advances") {
                UpdateState us;
                world.update (us);

                THEN ("The spaceship did not change") {
                    REQUIRE (ship.getPhysicalPosition () == ship_pos);
                    REQUIRE (ship.getPhysicalVelocity () == b2Vec2_zero);
                    REQUIRE (ship.getRotation () == ship_angle);
                }
            }
        }

        b2Body & body = * (b2Body *) ship.getUserData();
        auto mass = body.GetMass();
        WHEN ("A linear impulse is applied to the spaceship") {
            b2Vec2 impulse {0.1, 0};
            body.ApplyLinearImpulseToCenter (impulse, true);

            THEN ("The spaceship moves predictably linear") {
                for (int i = 1; i < TIME_STEPS; i++) {
                    UpdateState us;
                    world.update (us);

                    auto vel_estimate = (1 / mass) * impulse;
                    auto velocity = body.GetLinearVelocity ();
                    REQUIRE (vel_estimate.y == velocity.y);
                    REQUIRE (vel_estimate.x == velocity.x);

                    auto delta = (1.0 / FPS * i) * velocity;
                    REQUIRE (delta.y == 0);
                    REQUIRE (delta.x > 0);

                    auto pos_estimate = ship_pos + delta;
                    REQUIRE (pos_estimate.y == ship_pos.y);
                    REQUIRE (body.GetPosition ().y == ship_pos.y);
                    REQUIRE (closeEnough (body.GetPosition ().x, pos_estimate.x));

                    // Keep movement synchronised with ideal values
                    body.SetTransform (pos_estimate, body.GetAngle());
                }
            }
        }

        auto angle = body.GetAngle();
        WHEN ("An angular impulse is applied to the spaceship") {
            float impulse = 0.1;
            body.ApplyAngularImpulse (impulse, true);
            auto ang_velocity = body.GetAngularVelocity();
            auto delta = 1.0 / FPS * ang_velocity;

            THEN ("The spaceship rotates but the rotation slows down") {
                for (int i = 1; i < TIME_STEPS; i++) {
                    UpdateState us;
                    world.update (us);

                    auto new_ang_velocity = body.GetAngularVelocity();
                    REQUIRE (new_ang_velocity < ang_velocity);
                    REQUIRE (new_ang_velocity > 0);
                    ang_velocity = new_ang_velocity;

                    auto new_delta = 1.0 / FPS * ang_velocity;
                    REQUIRE (new_delta < delta);
                    REQUIRE (new_delta > 0);
                    delta = new_delta;
                }
            }
        }
    }
}

/* Copyright © 2022 Aaron Alef */
