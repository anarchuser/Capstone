#ifndef _CAPSTONE_BACKEND_H
#define _CAPSTONE_BACKEND_H

#include "config.h"
#include "Network/config.h"

#include "oxygine-framework.h"

#include "helper.h"
#include "Spaceship/KeyboardSpaceship.h"

#include <unordered_map>
#include <string>
#include <thread>

#define ITOS(I) std::to_string (I)
#define IP(first, second, third, fourth) (ITOS (first) +'.'+ ITOS (second) +'.'+ ITOS (third) +'.'+ ITOS (fourth))

namespace kt {
    using namespace oxygine;

    class Backend {
    private:
        std::size_t seed;

        std::thread server_thread;
        std::string const address;
        std::atomic <short> port = -1;

        std::atomic <bool> stop = false;

        void serve ();

    public:
        Backend (std::size_t seed, std::string address);
        ~Backend () noexcept;

        [[nodiscard]] unsigned short getPort() const;
        [[nodiscard]] std::string const & getAddress() const;

        static bool ping (std::string const & ip, short port);
    };

} // kt

#endif //_CAPSTONE_BACKEND_H

/* Copyright (C) Aaron Alef */