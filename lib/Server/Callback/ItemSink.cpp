#include "ItemSink.h"

namespace cg {
    ItemSinkImpl::ItemSinkImpl (Callbacks && callbacks) {
        callbacks.onDone.exchange     (callbacks.onDone);
        callbacks.onSendItem.exchange (callbacks.onSendItem);
    }

    void ItemSinkImpl::setOnDone     (DoneCallback onDone)         { callbacks.onDone.exchange     (onDone); }
    void ItemSinkImpl::setOnSendItem (SendItemCallback onSendItem) { callbacks.onSendItem.exchange (onSendItem); }

    void ItemSinkImpl::log (std::string const & msg) {
        std::stringstream ss;
        ss << "Callback @" << this << ": '" << msg << "'" << std::endl;
        KJ_LOG (INFO, ss.str());
    }

    ::kj::Promise<void> ItemSinkImpl::done (DoneContext context) {
        log ("Disconnect client");
        auto callback = callbacks.onDone.load();
        if (! callback.expired()) (* callback.lock())();
        else KJ_LOG (WARNING, "onDisconnect called without callback registered");
        return kj::READY_NOW;
    }

    ::kj::Promise<void> ItemSinkImpl::sendItem (SendItemContext context) {
        auto direction = context.getParams().getItem().getDirection();

        auto callback = callbacks.onSendItem.load();
        if (! callback.expired()) {
            (* callback.lock()) ({
                direction.getAccelerate(),
                direction.getDecelerate(),
                direction.getRotateLeft(),
                direction.getRotateRight()
            });
        }
        else KJ_LOG (WARNING, "onRequestItem called without callback registered");

        return kj::READY_NOW;
    }
}

/* Copyright © 2022 Aaron Alef */
