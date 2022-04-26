#include "Text.h"

namespace kt {
    Text::Text (ResFont * font, std::string message) {
        setText (message);

        TextStyle style (font);
//        style.withColor ({255, 255, 0, 0});
//        style.alignMiddle();
        setStyle (style);
    }
}

/* Copyright © 2022 Aaron Alef */
