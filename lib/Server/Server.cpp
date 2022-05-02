#include "Server.h"

namespace cg {

    std::function <void (Direction direction)> SynchroImpl::updateDirectionCallback;

    void SynchroImpl::log (std::string msg) {
        LOG (INFO) << "Synchro @" << this << ": '" << msg << "'";
        std::cout << "Synchro @" << this << ": '" << msg << "'" << std::endl;
    }

    kj::Promise <void> SynchroImpl::connect (ConnectContext context) {
        log ("Listening");
        return kj::READY_NOW;
    }

    kj::Promise <void> SynchroImpl::updateDirection (UpdateDirectionContext context) {
        auto direction = context.getParams().getDirection();
        try {
            updateDirectionCallback (
                    {direction.getAccelerate (), direction.getDecelerate (), direction.getRotateLeft (),
                     direction.getRotateRight ()
                    });
            std::string msg = "Update direction to ";
            if (direction.getRotateLeft() == 1 && direction.getRotateRight() != 1) msg += '<';
            else msg += ' ';
            if (direction.getAccelerate() == 1) msg += '^';
            else if (direction.getDecelerate() == 1) msg += '_';
            else msg += '*';
            if (direction.getRotateLeft() != 1 && direction.getRotateRight() == 1) msg += '>';
            else msg += ' ';
            log (msg);
        } catch (std::bad_function_call &) {
            LOG (WARNING) << "Received updateDirection before callback has been initialised";
        }
        return kj::READY_NOW;
    }

    ::kj::Promise<void> SynchroImpl::streamDirections (Synchro::Server::StreamDirectionsContext context) {
        log ("Add new projected spaceship");

        auto result = context.getResults();
        result.setCallback (kj::heap <DirectionCallbackImpl> (onNewRemoteSpaceship()));

        return kj::READY_NOW;
    }

    SynchroImpl::SynchroImpl (std::function<std::function <void (Direction)> ()> && onNewStreamCallback)
            : onNewRemoteSpaceship {std::move (onNewStreamCallback)} {}

    DirectionCallbackImpl::DirectionCallbackImpl (std::function<void (Direction)> && updateDirectionCallback)
            : updateDirectionCallback {std::move (updateDirectionCallback)} {}

    void DirectionCallbackImpl::log (std::string msg) {
        LOG (INFO) << "DirectionCallback @" << this << ": '" << msg << "'";
        std::cout << "DirectionCallback @" << this << ": '" << msg << "'" << std::endl;
    }

    ::kj::Promise<void>
    DirectionCallbackImpl::sendDirection (SendDirectionContext context) {
        auto direction = context.getParams().getDirection();
        try {
            updateDirectionCallback ({
                direction.getAccelerate (),
                direction.getDecelerate (),
                direction.getRotateLeft (),
                direction.getRotateRight ()
            });
//            std::string msg = "Update direction to ";
//            if (direction.getRotateLeft() == 1 && direction.getRotateRight() != 1) msg += '<';
//            else msg += ' ';
//            if (direction.getAccelerate() == 1) msg += '^';
//            else if (direction.getDecelerate() == 1) msg += '_';
//            else msg += '*';
//            if (direction.getRotateLeft() != 1 && direction.getRotateRight() == 1) msg += '>';
//            else msg += ' ';
//            log (msg);
        } catch (std::bad_function_call &) {
            LOG (WARNING) << "Received updateDirection before callback has been initialised";
        }
        return kj::READY_NOW;
    }

    ::kj::Promise<void> DirectionCallbackImpl::done (DoneContext context) {
        log ("Client disconnected");
        return kj::READY_NOW;
    }
}

/* Copyright © 2022 Aaron Alef */
