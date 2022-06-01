#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "Spaceship/Spaceship.h"

#define WINDOW_SIZE 100
#define TIME_STEPS 300

using namespace Catch;

SCENARIO ("Time passes in a near-empty world") {
    GIVEN ("A world containing one spaceship") {
        Stage::instance = new Stage;
        getStage()->setSize (WINDOW_SIZE, WINDOW_SIZE);

        b2Vec2 const world_size = {10, 10};
        kt::World world (nullptr, world_size);
        b2Vec2 const ship_pos = 0.5 * world_size;
        auto & ship = * new kt::Spaceship (world, nullptr, "");
        auto const ship_angle = ship.getRotation ();

        REQUIRE (ship.getPhysicalPosition () == ship_pos);
        REQUIRE (ship.getPhysicalVelocity () == b2Vec2_zero);

        WHEN ("Time advances") {
            THEN ("The spaceship does not change") {
                for (int i = 0; i < TIME_STEPS; i++) {
                    UpdateState us;
                    ship.update (us);

                    CHECK (ship.getPhysicalPosition () == ship_pos);
                    CHECK (ship.getPhysicalVelocity () == b2Vec2_zero);
                    CHECK (ship.getRotation () == ship_angle);
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
                    UpdateState us; us.dt = 10;
                    ship.update (us);

                    auto vel_estimate = (1 / mass) * impulse;
                    auto velocity = body.GetLinearVelocity ();
                    CHECK (vel_estimate.y == velocity.y);
                    CHECK (vel_estimate.x == velocity.x);

                    auto delta = (1e-3 * us.dt) * i * velocity;
                    CHECK (delta.y == 0);
                    CHECK (delta.x > 0);

                    auto pos_estimate = ship_pos + delta;
                    CHECK (pos_estimate.y == ship_pos.y);
                    CHECK (body.GetPosition ().y == ship_pos.y);
                    CHECK (body.GetPosition ().x == Approx (pos_estimate.x).epsilon (2e-2));

                    // Keep movement synchronised with ideal values
                    body.SetTransform (pos_estimate, body.GetAngle ());
                }
            }
        }

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
                    CHECK (new_ang_velocity < ang_velocity);
                    CHECK (new_ang_velocity > 0);
                    ang_velocity = new_ang_velocity;

                    auto new_delta = 1.0 / FPS * ang_velocity;
                    CHECK (new_delta < delta);
                    CHECK (new_delta > 0);
                    delta = new_delta;
                }
            }
        }
    }
}

/* Copyright © 2022 Aaron Alef */
