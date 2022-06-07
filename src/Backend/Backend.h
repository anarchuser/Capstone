#ifndef _CAPSTONE_BACKEND_H
#define _CAPSTONE_BACKEND_H

#include "Network/config.h"
#include "config.h"
#include "env.h"

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

    /// Wrapper around the Cap'n Proto server instance acting as actual backend
    class Backend {
    private:
        /// Seed of this game's instance
        std::size_t const seed;

        /// Thread the actual server is running on
        std::thread server_thread;
        /// Address the server is bound to
        std::string const address;
        /// Port the server is listening to
        std::atomic <short> port = -1;

        /// Flag to tell the server to stop
        std::atomic <bool> stop = false;

        /// Start the server on the server_thread
        void serve ();

    public:
        /// Start a new server
        Backend (std::size_t seed, std::string address);
        /// Tell the server thread to stop and wait until it did so
        ~Backend ();

        /// Wait for the server port to resolve to a positive value indicating the server is up and running
        unsigned short getPort() const;

        /// Send a ping to the given address and return success status
        static bool ping (std::string const & ip, short port);
    };

} // kt

#endif //_CAPSTONE_BACKEND_H

/* Copyright (C) Aaron Alef */