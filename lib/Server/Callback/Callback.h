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
    // TODO: Dummy struct
    struct State;

    template <class R, class... Args>
    using atomic_weak_fun = std::atomic <std::weak_ptr <std::function <R (Args...)>>>;
    typedef atomic_weak_fun <void>        DisconnectCallback;
    typedef atomic_weak_fun <Direction>   RequestItemCallback;
    typedef atomic_weak_fun <State>       RequestStateCallback;
    typedef atomic_weak_fun <void, State> UpdateStateCallback;

    class CallbackDirectionImpl final: public Synchro::Callback::Server {
    public:
        struct Callbacks {
            DisconnectCallback   onDisconnect;
            RequestItemCallback  onRequestItem;
            RequestStateCallback onRequestState;
            UpdateStateCallback  onUpdateState;
        };

        CallbackDirectionImpl () = default;
        explicit CallbackDirectionImpl (Callbacks && callbacks);

        /// Local function calls
        void setOnDisconnect   (DisconnectCallback   onDisconnect);
        void setOnRequestItem  (RequestItemCallback  onRequestItem);
        void setOnRequestState (RequestStateCallback onRequestState);
        void setOnUpdateState  (UpdateStateCallback  onUpdateState);

        /// RPC function calls
        ::kj::Promise <void> disconnect   (DisconnectContext context)   override;
        ::kj::Promise <void> requestItem  (RequestItemContext context)  override;
        ::kj::Promise <void> requestState (RequestStateContext context) override;
        ::kj::Promise <void> updateState  (UpdateStateContext context)  override;

    private:
        Callbacks callbacks;

        void log (std::string const & msg);
    };

} // cg

#endif //CAPSTONE_DIRECTIONCALLBACK_H

/* Copyright © 2022 Aaron Alef */
