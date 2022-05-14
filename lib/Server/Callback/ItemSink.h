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

#include "Data/Direction.h"

namespace cg {
    typedef std::function <void ()> DoneCallback;
    typedef std::function <void (Direction)> SendItemCallback;

    class ItemSinkImpl final: public Synchro::ItemSink::Server {
    private:
        struct Callbacks {
            DoneCallback     onDone;
            SendItemCallback onSendItem;
        } callbacks;

        void log (std::string const & msg);

    public:
        ItemSinkImpl (DoneCallback const & onDone, SendItemCallback const & onSendItem);
        ItemSinkImpl (DoneCallback && onDone, SendItemCallback && onSendItem);
        ItemSinkImpl() = default;
       ~ItemSinkImpl() = default;

        /// Local function calls
        void setOnDone     (DoneCallback     const & onDone);
        void setOnDone     (DoneCallback          && onDone);
        void setOnSendItem (SendItemCallback const & onSendItem);
        void setOnSendItem (SendItemCallback      && onSendItem);

        /// RPC function calls
        ::kj::Promise <void> done     (DoneContext     context) override;
        ::kj::Promise <void> sendItem (SendItemContext context) override;

    };

} // cg

#endif //CAPSTONE_ITEMSINK_H

/* Copyright © 2022 Aaron Alef */
