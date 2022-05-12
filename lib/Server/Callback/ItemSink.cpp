#include "ItemSink.h"

namespace cg {
    ItemSinkImpl::ItemSinkImpl (DoneCallback && onDone, SendItemCallback && onSendItem)
            : callbacks {std::move (onDone), std::move (onSendItem)} {}
    ItemSinkImpl::ItemSinkImpl (DoneCallback const & onDone, SendItemCallback const & onSendItem)
            : callbacks {onDone, onSendItem} {}

    void ItemSinkImpl::setOnDone     (DoneCallback     const & onDone)     { callbacks.onDone     = onDone; }
    void ItemSinkImpl::setOnDone     (DoneCallback     && onDone)     { callbacks.onDone     = std::move (onDone); }
    void ItemSinkImpl::setOnSendItem (SendItemCallback const & onSendItem) { callbacks.onSendItem = onSendItem; }
    void ItemSinkImpl::setOnSendItem (SendItemCallback && onSendItem) { callbacks.onSendItem = std::move (onSendItem); }

    void ItemSinkImpl::log (std::string const & msg) {
        std::stringstream ss;
        ss << "ItemSink @" << this << ": '" << msg << "'";
        KJ_DLOG (INFO, ss.str());
        std::cout << ss.str() << std::endl;
    }

    ::kj::Promise<void> ItemSinkImpl::done (DoneContext context) {
        log ("Disconnect client");

        try {
            callbacks.onDone();
        } catch (std::bad_function_call & e) {
            KJ_DLOG (WARNING, "ItemSink::done called without valid callback registered");
        }

        return kj::READY_NOW;
    }

    ::kj::Promise<void> ItemSinkImpl::sendItem (SendItemContext context) {
        try {
            auto direction = context.getParams ().getItem ().getDirection ();
            callbacks.onSendItem ({
                direction.getAccelerate (),
                direction.getDecelerate (),
                direction.getRotateLeft (),
                direction.getRotateRight ()
            });
        } catch (std::bad_function_call & e) {
//            KJ_DLOG (WARNING, "ItemSink::sendItem called without valid callback registered");
        }

        return kj::READY_NOW;
    }
}

/* Copyright © 2022 Aaron Alef */
