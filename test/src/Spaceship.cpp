#include <catch2/catch_test_macros.hpp>

#include "Algebra/algebra.h"

#include "Spaceship/Spaceship.h"

#define TIME_STEPS 61
#define WINDOW_SIZE 100

SCENARIO ("Time passes in a near-empty world") {
    GIVEN ("A world containing one spaceship") {
        b2Vec2 world_size = {10, 10};
        Stage::instance = new Stage;
        getStage()->setSize (WINDOW_SIZE, WINDOW_SIZE);

        kt::World world (nullptr, {10, 10});
        b2Vec2 ship_pos = 0.5 * world_size;
        auto & ship = * new kt::Spaceship (world, nullptr, world.convert (ship_pos), 1);
        auto ship_angle = ship.getRotation ();

        REQUIRE (ship.getPhysicalPosition () == ship_pos);
        REQUIRE (ship.getPhysicalVelocity () == b2Vec2_zero);

        for (int i = 0; i < TIME_STEPS; i++) {
            WHEN ("Time advances") {
                UpdateState us;
                ship.update (us);

                THEN ("The spaceship did not change") {
                    REQUIRE (ship.getPhysicalPosition () == ship_pos);
                    REQUIRE (ship.getPhysicalVelocity () == b2Vec2_zero);
                    REQUIRE (ship.getRotation () == ship_angle);
                }
            }
        }

        b2Body & body = * (b2Body *) ship.getUserData ();
        auto mass = body.GetMass ();
        WHEN ("A linear impulse is applied to the spaceship") {
            b2Vec2 impulse {0.1, 0};
            body.ApplyLinearImpulseToCenter (impulse, true);

            THEN ("The spaceship moves predictably linear") {
                for (int i = 1; i < TIME_STEPS; i++) {
                    UpdateState us;
                    ship.update (us);

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
//                    REQUIRE (closeEnough (body.GetPosition ().x, pos_estimate.x));

                    // Keep movement synchronised with ideal values
                    body.SetTransform (pos_estimate, body.GetAngle ());
                }
            }
        }

        auto angle = body.GetAngle ();
        WHEN ("An angular impulse is applied to the spaceship") {
            float impulse = 0.1;
            body.ApplyAngularImpulse (impulse, true);
            auto ang_velocity = body.GetAngularVelocity ();
            auto delta = 1.0 / FPS * ang_velocity;

            THEN ("The spaceship rotates but the rotation slows down") {
                for (int i = 1; i < TIME_STEPS; i++) {
                    UpdateState us;
                    ship.update (us);

                    auto new_ang_velocity = body.GetAngularVelocity ();
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
