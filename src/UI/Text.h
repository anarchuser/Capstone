#ifndef CAPSTONE_TEXT_H
#define CAPSTONE_TEXT_H

#include "config.h"

#include "oxygine-framework.h"

#include <string>

namespace kt {
    using namespace oxygine;

    /// Wrapper around TextField that takes font and message directly as (optional) arguments
    class Text: public TextField {
    public:
        /// Create a new Text field with given font and message. Text fields without font may not be rendered correctly
        explicit Text (ResFont * font = nullptr, std::string const & message = "");

    };
    DECLARE_SMART (Text, spText);

} // kt

#endif //CAPSTONE_TEXT_H

/* Copyright © 2022 Aaron Alef */
