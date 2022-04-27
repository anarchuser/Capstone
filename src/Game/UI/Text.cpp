#include "Text.h"

namespace kt {
    Text::Text (ResFont * font, std::string message) {
        setText (message);

        if (font) {
            TextStyle style (font);
            setStyle (style);
        }
    }
}

/* Copyright © 2022 Aaron Alef */
