#ifndef CAPSTONE_DIRECTIONCALLBACK_H
#define CAPSTONE_DIRECTIONCALLBACK_H

#include "config.h"

#include "Server/generated/synchro.capnp.h"

#include <kj/debug.h>
#include <kj/memory.h>
#include <capnp/ez-rpc.h>
#include <capnp/message.h>

#include <atomic>
#include <functional>
#include <iostream>
#include <memory>

#include "Direction/Direction.h"

namespace cg {
    template <class R, class... Args>
    using atomic_weak_fun = std::atomic <std::weak_ptr <std::function <R (Args...)>>>;

    class CallbackDirectionImpl final: public Synchro::Callback <Direction, void>::Server {
    private:
        atomic_weak_fun <Direction> onRequestItem;
        atomic_weak_fun <void> onRequestState;
        atomic_weak_fun <void> onDone;

        void log (std::string const & msg);

    public:
        /// Local function calls
        void setOnRequestItem (atomic_weak_fun <Direction> onRequestItem);
        void setOnRequestState (atomic_weak_fun <void> onRequestState);
        void setOnDone (atomic_weak_fun <void> onDone);

        /// RPC function calls
        ::kj::Promise <void> requestItem (RequestItemContext context) override;
        ::kj::Promise <void> requestState (RequestStateContext context) override;
        ::kj::Promise <void> done (DoneContext context) override;
    };

} // cg

#endif //CAPSTONE_DIRECTIONCALLBACK_H

/* Copyright © 2022 Aaron Alef */
