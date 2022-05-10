#include "ItemSink.h"

namespace cg {
    ItemSinkImpl::ItemSinkImpl (DoneCallback & onDone, SendItemCallback & onSendItem)
            : callbacks {onDone, onSendItem} {}

    void ItemSinkImpl::setOnDone     (DoneCallback && onDone)         { callbacks.onDone     = std::move (onDone); }
    void ItemSinkImpl::setOnSendItem (SendItemCallback && onSendItem) { callbacks.onSendItem = std::move (onSendItem); }

    void ItemSinkImpl::log (std::string const & msg) {
        std::stringstream ss;
        ss << "Callback @" << this << ": '" << msg << "'" << std::endl;
        KJ_DLOG (INFO, ss.str());
    }

    ::kj::Promise<void> ItemSinkImpl::done (DoneContext context) {
        log ("Disconnect client");
        auto ptr = callbacks.onDone;
        if (! ptr.expired()) {
            auto callback = ptr.lock();
            if (callback) (* callback) ();
            else KJ_DLOG (WARNING, "onDone called without callback registered");
        } else KJ_DLOG (WARNING, "onDone called whilst callback expired");
        return kj::READY_NOW;
    }

    ::kj::Promise<void> ItemSinkImpl::sendItem (SendItemContext context) {
        log ("Received direction");

        auto direction = context.getParams().getItem().getDirection();

        auto ptr = callbacks.onSendItem;
        if (auto callback = ptr.lock()) {
            (* callback) ({
                direction.getAccelerate(),
                direction.getDecelerate(),
                direction.getRotateLeft(),
                direction.getRotateRight()
            });
        } else KJ_DLOG (WARNING, "onRequestItem called whilst callback expired");

        return kj::READY_NOW;
    }
}

/* Copyright © 2022 Aaron Alef */
