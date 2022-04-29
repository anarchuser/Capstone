#ifndef CAPSTONE_SERVER_H
#define CAPSTONE_SERVER_H

#include "config.h"

#include "generated/synchro.capnp.h"

#include <kj/debug.h>
#include <kj/memory.h>
#include <capnp/ez-rpc.h>
#include <capnp/message.h>

#include <iostream>

/* Following Cap'n Proto Server example:
 * https://github.com/capnproto/capnproto/blob/master/c%2B%2B/samples/calculator-server.c%2B%2B
 */

namespace cg {

    class SynchroImpl final: public Synchro::Server {
    private:
        void log (std::string msg);

    public:
        kj::Promise <void> connect (ConnectContext context) override;

        kj::Promise <void> updateDirection (UpdateDirectionContext context) override;
    };

} // cg

#endif //CAPSTONE_SERVER_H

/* Copyright © 2022 Aaron Alef */
