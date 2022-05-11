#include "ItemSink.h"

namespace cg {
    ItemSinkImpl::ItemSinkImpl (DoneCallback & onDone, SendItemCallback & onSendItem)
            : callbacks {onDone, onSendItem} {}

    void ItemSinkImpl::setOnDone     (DoneCallback     const & onDone)     { callbacks.onDone     = onDone; }
    void ItemSinkImpl::setOnSendItem (SendItemCallback const & onSendItem) { callbacks.onSendItem = onSendItem; }

    void ItemSinkImpl::log (std::string const & msg) {
        std::stringstream ss;
        ss << "ItemSink @" << this << ": '" << msg << "'";
        KJ_DLOG (INFO, ss.str());
        std::cout << ss.str() << std::endl;
    }

    ::kj::Promise<void> ItemSinkImpl::done (DoneContext context) {
        log ("Disconnect client");

        callbacks.onDone();

//        auto ptr = callbacks.onDone;
//        if (! ptr.expired()) {
//            auto callback = ptr.lock();
//            if (callback) (* callback) ();
//            else KJ_DLOG (WARNING, "onDone called without callback registered");
//        } else KJ_DLOG (WARNING, "onDone called whilst callback expired");
        return kj::READY_NOW;
    }

    ::kj::Promise<void> ItemSinkImpl::sendItem (SendItemContext context) {
//        log ("Received direction");

        auto direction = context.getParams().getItem().getDirection();
        callbacks.onSendItem ({
            direction.getAccelerate(),
            direction.getDecelerate(),
            direction.getRotateLeft(),
            direction.getRotateRight()
        });

//        auto ptr = callbacks.onSendItem;
//        if (auto callback = ptr.lock()) {
//            (* callback) ({
//                direction.getAccelerate(),
//                direction.getDecelerate(),
//                direction.getRotateLeft(),
//                direction.getRotateRight()
//            });
//        } else KJ_DLOG (WARNING, "onRequestItem called whilst callback expired");

        return kj::READY_NOW;
    }
}

/* Copyright © 2022 Aaron Alef */
