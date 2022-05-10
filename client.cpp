#include "config.h"

#include "Server/generated/synchro.capnp.h"
#include <capnp/ez-rpc.h>
#include <kj/debug.h>

#include <iostream>

void updateDirection (Synchro::Direction::Builder && direction, std::string directions) {
    direction.setAccelerate (false);
    direction.setDecelerate (false);
    direction.setRotateLeft (false);
    direction.setRotateRight (false);
    for (char c : directions) {
        switch (c) {
            case 'w':
            case 'W':
                direction.setAccelerate (true);
                break;
            case 'a':
            case 'A':
                direction.setRotateLeft (true);
                break;
            case 's':
            case 'S':
                direction.setDecelerate (true);
                break;
            case 'd':
            case 'D':
                direction.setRotateRight (true);
                break;
        }
    }
    std::string msg = "Direction: ";
    if (direction.getRotateLeft() && !direction.getRotateRight()) msg += '<';
    else msg += ' ';
    if (direction.getAccelerate()) msg += '^';
    else if (direction.getDecelerate()) msg += '_';
    else msg += '*';
    if (!direction.getRotateLeft() && direction.getRotateRight()) msg += '>';
    else msg += ' ';
    std::cout << msg << std::endl;
}

int main (int argc, char * argv[]) {
    std::cout << "Simple client for testing RpcServer connections" << std::endl;

    std::string address = "localhost";
    std::size_t port = 44444;

    auto rpcClient = kj::heap <capnp::EzRpcClient> (address, port);
    Synchro::Client client = rpcClient->getMain <Synchro> ();
    auto & waitScope = rpcClient->getWaitScope();

    client.pingRequest().send().then ([] (...) {
        std::cout << "Connect request finished" << std::endl;
    }).wait (rpcClient->getWaitScope());

    std::string query;
    while (true) {
        (std::cout << "$ ").flush();

        // Request input
        getline (std::cin, query);
        if (query.empty()) continue;

        if (query == "exit") {
            std::cout << "Exit client requested";
            return EXIT_SUCCESS;
        }

        if (query == "ping") {
            client.pingRequest().send().wait (waitScope);
            continue;
        }

        try {
            auto request = client.pingRequest();
            request.send().wait (waitScope);
        } catch (std::exception & e) {
            std::cout << "Server: '" << e.what() << "'" << std::endl;
        }
    }
}

/* Copyright © 2022 Aaron Alef */
