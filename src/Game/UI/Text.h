#ifndef CAPSTONE_TEXT_H
#define CAPSTONE_TEXT_H

#include "config.h"

#include "oxygine-framework.h"

#include <string>

namespace kt {
    using namespace oxygine;

    class Text: public TextField {
    public:
        Text (ResFont * font = nullptr, std::string message = "");

    };
    DECLARE_SMART (Text, spText);

} // kt

#endif //CAPSTONE_TEXT_H

/* Copyright © 2022 Aaron Alef */
