#ifndef CAPSTONE_BUTTON_H
#define CAPSTONE_BUTTON_H

#include "config.h"

#include "oxygine-framework.h"

#include <string>
#include <functional>

namespace kt {
    using namespace oxygine;

    class Button: public Sprite {
        Button (ResAnim * animation, std::string message, std::function <void()> onAction);
    };

} // ky

#endif //CAPSTONE_BUTTON_H

/* Copyright © 2022 Aaron Alef */
