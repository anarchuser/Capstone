/* Copyright (C) 2022 Aaron Alef <aaron.alef@code.berlin>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (e.g., ./LICENSE).
 * If not, see <https://www.gnu.org/licenses/>.
 */

#include "ox/oxygine.hpp"
#include "ox/Stage.hpp"
#include "ox/DebugActor.hpp"

//#include "example/example.h"
#include "click_box/click_box.h"
using namespace cb;

#include <iostream>

#include "config.h"

using namespace oxygine;

int setupGlog (int argc, char * argv[]) {
    google::InitGoogleLogging (argv[0]);
}

// This function is called each frame
int mainloop()
{
    // Update engine-internal components
    // If input events are available, they are passed to Stage::instance.handleEvent
    // If the function returns true, it means that the user requested the application to terminate
    bool done = core::update();

    // It gets passed to our click_box game implementation
    click_box_update();

    // Update our stage
    // Update all actors. Actor::update will also be called for all its children
    getStage()->update();

    if (core::beginRendering())
    {
        Color clearColor(32, 32, 32, 255);
        Rect viewport(Point(0, 0), core::getDisplaySize());
        // Render all actors inside the stage. Actor::render will also be called for all its children
        getStage()->render(clearColor, viewport);

        core::swapDisplayBuffers();
    }

    return done ? 1 : 0;
}

// Application entry point
void run()
{
    ObjectBase::__startTracingLeaks();

    // Initialize Oxygine's internal stuff
    core::init_desc desc;
    desc.title = "Oxygine Application";

    // The initial window size can be set up here on SDL builds, ignored on Mobile devices
    desc.w = 960;
    desc.h = 640;


    click_box_preinit();
    core::init(&desc);


    // Create the stage. Stage is a root node for all updateable and drawable objects
    Stage::instance = new Stage();
    Point size = core::getDisplaySize();
    getStage()->setSize(size);

    // DebugActor is a helper actor node. It shows FPS, memory usage and other useful stuff
    DebugActor::show();

    // Initializes our click_box game. See click_box.cpp
    click_box_init();

#ifdef EMSCRIPTEN
    /*
    If you build for Emscripten, mainloop is called automatically and shouldn't be called here.
    See emscripten_set_main_loop in the EMSCRIPTEN section below
    */
    return;
#endif



#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
    // On iPhone mainloop is called automatically by CADisplayLink, see int main() below
    //return;
#endif

    // This is the main game loop.
    while (1)
    {
        int done = mainloop();
        if (done)
            break;
    }
    /*
     If we get here, the user has requested the Application to terminate.
     We dump and log all our created objects that have not been freed yet
    */
    ObjectBase::dumpCreatedObjects();

    /*
    Let's clean up everything right now and call ObjectBase::dumpObjects() again.
    We need to free all allocated resources and delete all created actors.
    All actors/sprites are smart-pointer objects and don't need to be removed by hand.
    But now we want to delete it by hand.
    */

    // See click_box.cpp for the shutdown function implementation
    click_box_destroy();


    //renderer.cleanup();

    // Releases all internal components and the stage
    core::release();

    // The dump list should be empty by now,
    // we want to make sure that there aren't any memory leaks, so we call it again.
    ObjectBase::dumpCreatedObjects();

    ObjectBase::__stopTracingLeaks();
    //end
}

#ifdef OXYGINE_SDL

#include "SDL_main.h"
#include "SDL.h"

extern "C"
{
    void one(void* param) { mainloop(); }
    void oneEmsc() { mainloop(); }

    int main(int argc, char* argv[])
    {
        setupGlog (argc, argv);
        run();

#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
        // If parameter 2 is set to 1, refresh rate will be 60 fps, 2 - 30 fps, 3 - 15 fps.
        //SDL_iPhoneSetAnimationCallback(core::getWindow(), 1, one, nullptr);
#endif

#if EMSCRIPTEN
        emscripten_set_main_loop(oneEmsc, 0, 0);
#endif

        return 0;
    }
};
#endif