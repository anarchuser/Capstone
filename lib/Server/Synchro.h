#ifndef CAPSTONE_SYNCHRO_H
#define CAPSTONE_SYNCHRO_H

#include "config.h"

#include "lib/Server/generated/synchro.capnp.h"
#include <kj/debug.h>
#include <kj/memory.h>
#include <capnp/ez-rpc.h>
#include <capnp/message.h>

#include <atomic>
#include <functional>
#include <iostream>
#include <sstream>
#include <memory>
#include <unordered_map>

#include <ranges>

#include "Data/Direction.h"
#include "Data/Spaceship.h"
#include "ItemSink.h"
#include "ShipCallback.h"

/* Following Cap'n Proto Server example:
 * https://github.com/capnproto/capnproto/blob/master/c%2B%2B/samples/calculator-server.c%2B%2B
 */

namespace cg {
    class SynchroImpl final: public Synchro::Server {
    private:
        struct Connection {
            struct CallbackHandles {
                SendItemCallback sendItemCallbackHandle;
                DoneCallback     doneCallbackHandle;
            } handles;

            Synchro::ShipCallback::Client shipCallback;
            std::unordered_map <std::string, Synchro::ItemSink::Client> itemSinks;

            explicit Connection (CallbackHandles && handle, Synchro::ShipCallback::Client && shipCallback);
        };

        /// List of open connections per username
        //TODO: replace username with proper UUID
        std::unordered_map <std::string, Connection> connections;

        /// Seed used to initialise the game. Returned by `randomSeed`
        std::size_t const rng_seed;

        void sendItemCallback (std::string const & sender, Direction direction);
        void doneCallback     (std::string username);

        void distributeSpaceship (Spaceship const & sender, std::string const & receiver);
        void broadcastSpaceship  (Spaceship const & sender);

        void log (std::string const & msg);

    public:
        explicit SynchroImpl (std::size_t seed);

        /// RPC function calls
        ::kj::Promise <void> ping (PingContext context) override;
        ::kj::Promise <void> seed (SeedContext context) override;
        ::kj::Promise <void> join (JoinContext context) override;
    };
} // cg

#endif //CAPSTONE_SYNCHRO_H

/* Copyright © 2022 Aaron Alef */