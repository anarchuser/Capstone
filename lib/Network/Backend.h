#ifndef CAPSTONE_NETWORK_BACKEND_H
#define CAPSTONE_NETWORK_BACKEND_H

#include "Network/config.h"

#include "Synchro/Synchro.h"
#include "Registrar/Registrar.h"
#include "ShipSink/ShipSink.h"
#include "ShipHandle/ShipHandle.h"

#include "Data/Direction.h"
#include "Data/Spaceship.h"

#include <atomic>
#include <functional>
#include <iostream>
#include <sstream>
#include <memory>
#include <unordered_map>
#include <vector>
#include <numeric>
#include <ranges>
#include <iterator>

/* Following Cap'n Proto Server example:
 * https://github.com/capnproto/capnproto/blob/master/c%2B%2B/samples/calculator-server.c%2B%2B
 */

namespace cg {
    using namespace std::string_literals;

    class BackendImpl final: public Backend::Server {
    private:
        /// Seed of random number generator of currently running game
        std::size_t const rng_seed;

        /// Own identifier
        std::string const ID;

        /// Struct holding all kinds of information about things connected
        struct Client {
            /// Whether it is a local game client or a remote synchro backend (redundant; same as maybe <synchro>)
            enum Type { LOCAL, REMOTE } type;

            /// Thing used to update the client (e.g., new ship spawned)
            Backend::Registrar::Client registrar;

            /// If the client is a remote backend use this to interact with it
            kj::Maybe <Backend::Synchro::Client> synchro;

            /// List of handles to all ships in possession registered by this client
            std::unordered_map <std::string, Backend::ShipHandle::Client> ships;

            /// List of all ship sinks this client needs to distribute incoming events to
            std::unordered_map <std::string, Backend::ShipSink::Client> sinks;

            Client (Backend::Registrar::Client && registrar, kj::Maybe <Backend::Synchro::Client> && synchro, Type type = REMOTE);
            explicit Client (Backend::Registrar::Client && registrar);
        };
        /// List of all clients connected
        std::unordered_map <std::string, Client> clients;

        /// Synchro callbacks
        kj::Own <RegistrarImpl> connectCallback (std::string const & id, Backend::Synchro::Client synchro, Backend::Registrar::Client remoteRegistrar);

        /// RegisterShip callback
        kj::Own <ShipSinkImpl> registerShip    (Spaceship const & ship, std::string const & id, Backend::ShipHandle::Client);
        kj::Promise <void> broadcastSpaceship  (Spaceship const & ship);
        kj::Promise <void> distributeSpaceship (Spaceship const & ship, Client & receiver);

        /// ShipSink callbacks
        kj::Promise <void> doneCallback     (std::string const & username);
        kj::Promise <void> sendItemCallback (std::string const & username, Direction const & direction, std::string const & id);
        kj::Promise <void> sendItemToClient (std::string const & username, Direction const & direction, Client & receiver);

        /// Log function of this implementation
        void log (std::string const & msg);

    public:
        explicit BackendImpl (std::size_t seed, std::string id);

        /** RPC function calls **/
        ::kj::Promise <void> ping    (PingContext    context) override;
        ::kj::Promise <void> seed    (SeedContext    context) override;
        ::kj::Promise <void> connect (ConnectContext context) override;
        ::kj::Promise <void> join    (JoinContext    context) override;
    };
} // cg

#endif //CAPSTONE_NETWORK_BACKEND_H

/* Copyright © 2022 Aaron Alef */
