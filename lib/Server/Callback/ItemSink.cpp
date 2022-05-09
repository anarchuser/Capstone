#include "ItemSink.h"

namespace cg {
    CallbackDirectionImpl::CallbackDirectionImpl (Callbacks && callbacks) {
        callbacks.onDisconnect.exchange   (callbacks.onDisconnect);
        callbacks.onRequestItem.exchange  (callbacks.onRequestItem);
        callbacks.onRequestState.exchange (callbacks.onRequestState);
        callbacks.onUpdateState.exchange  (callbacks.onUpdateState);
    }

    void CallbackDirectionImpl::setOnDisconnect (DisconnectCallback onDisconnect) {
        callbacks.onDisconnect.exchange (onDisconnect);
    }
    void CallbackDirectionImpl::setOnRequestItem (RequestItemCallback onRequestItem) {
        callbacks.onRequestItem.exchange (onRequestItem);
    }
    void CallbackDirectionImpl::setOnRequestState (RequestStateCallback onRequestState) {
        callbacks.onRequestState.exchange (onRequestState);
    }
    void CallbackDirectionImpl::setOnUpdateState (UpdateStateCallback onUpdateState) {
        callbacks.onUpdateState.exchange (onUpdateState);
    }

    void CallbackDirectionImpl::log (std::string const & msg) {
        std::stringstream ss;
        ss << "Callback @" << this << ": '" << msg << "'" << std::endl;
        KJ_LOG (INFO, ss.str());
    }

    ::kj::Promise<void> CallbackDirectionImpl::disconnect (DisconnectContext context) {
        log ("Disconnect client");
        auto callback = callbacks.onDisconnect.load();
        if (! callback.expired()) (* callback.lock())();
        else KJ_LOG (WARNING, "onDisconnect called without callback registered");
        return kj::READY_NOW;
    }

    ::kj::Promise<void> CallbackDirectionImpl::requestItem (RequestItemContext context) {
        auto builder = context.initResults().initItem();

        auto callback = callbacks.onRequestItem.load();
        if (! callback.expired()) {
            auto direction = (* callback.lock())();
            builder.setAccelerate  (direction.accelerate);
            builder.setDecelerate  (direction.decelerate);
            builder.setRotateLeft  (direction.rotateLeft);
            builder.setRotateRight (direction.rotateRight);
        }
        else KJ_LOG (WARNING, "onRequestItem called without callback registered");

        return kj::READY_NOW;
    }

    ::kj::Promise<void> CallbackDirectionImpl::requestState (RequestStateContext context) {
        KJ_UNIMPLEMENTED();

        auto builder = context.initResults().initState();

        auto callback = callbacks.onRequestState.load();
        if (! callback.expired()) {
//            auto state = (* callback.lock())();
            builder.setNothing();
        }
        else KJ_LOG (WARNING, "onRequestState called without callback registered");

        return kj::READY_NOW;
    }

    ::kj::Promise<void> CallbackDirectionImpl::updateState (UpdateStateContext context) {
        KJ_UNIMPLEMENTED();

        auto reader = context.getParams().getState();

        auto callback = callbacks.onUpdateState.load();
        if (! callback.expired()) {
            State * state = nullptr;  /* TODO convert reader to state here */
//            (* callback.lock()) (state);
        }
        else KJ_LOG (WARNING, "onUpdateState called without callback registered");

        return kj::READY_NOW;
    }
}

/* Copyright © 2022 Aaron Alef */
