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

#include "config.h"

#include "Game/Game.h"

#include "Server/Server.h"
#include <capnp/ez-rpc.h>
#include <kj/debug.h>

#include <iostream>
#include <string>

#define DEFAULT_ADDRESS "localhost"
#define DEFAULT_PORT    "44444"

kj::Own <capnp::EzRpcServer> startServer (std::string address) {
    logs::message (" Setting up game backend at '%s'...", address.c_str());
    auto server = kj::heap <capnp::EzRpcServer> (kj::heap <cg::SynchroImpl>(), address);
    server->getPort().wait (server->getWaitScope());
    std::cout << " Done" << std::endl;
    return server;
}

int main (int argc, char * argv[]) {
    google::InitGoogleLogging (argv[0]);

    LOG (INFO) << "Start game client";

    // Start server, runs in background
    std::string address = DEFAULT_ADDRESS ":" DEFAULT_PORT;
    auto server = startServer (address);

    // Start game, runs endlessly
    kt::Game::run();

    LOG (INFO) << "Game client finished";
}
