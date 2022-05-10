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
    template <class R, class... Args>
    using   weak_fun = std::weak_ptr <std::function <R (Args...)>>;
    typedef weak_fun <void>            DoneCallback;
    typedef weak_fun <void, Direction> SendItemCallback;

    class ItemSinkImpl final: public Synchro::ItemSink::Server {
    private:
        struct Callbacks {
            DoneCallback     onDone;
            SendItemCallback onSendItem;
        } callbacks;

        void log (std::string const & msg);

    public:
        ItemSinkImpl (DoneCallback & onDone, SendItemCallback & onSendItem);
        ItemSinkImpl() = default;
        ~ItemSinkImpl() = default;

        /// Local function calls
        void setOnDone     (DoneCallback     && onDone);
        void setOnSendItem (SendItemCallback && onSendItem);

        /// RPC function calls
        ::kj::Promise <void> done     (DoneContext     context) override;
        ::kj::Promise <void> sendItem (SendItemContext context) override;

    };

} // cg

#endif //CAPSTONE_ITEMSINK_H

/* Copyright © 2022 Aaron Alef */
