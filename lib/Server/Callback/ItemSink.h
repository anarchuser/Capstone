#ifndef CAPSTONE_ITEMSINK_H
#define CAPSTONE_ITEMSINK_H

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
    // TODO: Dummy struct
    struct State;

    template <class R, class... Args>
    using atomic_weak_fun = std::atomic <std::weak_ptr <std::function <R (Args...)>>>;
    typedef atomic_weak_fun <void>        DisconnectCallback;
    typedef atomic_weak_fun <Direction>   RequestItemCallback;

    class CallbackDirectionImpl final: public Synchro::Callback::Server {
    public:
        struct Callbacks {
            DisconnectCallback   onDisconnect;
            RequestItemCallback  onRequestItem;
        };

        CallbackDirectionImpl () = default;
        explicit CallbackDirectionImpl (Callbacks && callbacks);
        ~CallbackDirectionImpl() = default;

        /// Local function calls
        void setOnDisconnect   (DisconnectCallback   onDisconnect);
        void setOnRequestItem  (RequestItemCallback  onRequestItem);

        /// RPC function calls
        ::kj::Promise <void> disconnect   (DisconnectContext context)   override;
        ::kj::Promise <void> requestItem  (RequestItemContext context)  override;

    private:
        Callbacks callbacks;

        void log (std::string const & msg);
    };

} // cg

#endif //CAPSTONE_ITEMSINK_H

/* Copyright © 2022 Aaron Alef */
