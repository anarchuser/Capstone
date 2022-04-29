#include "Server.h"

namespace cg {

    void SynchroImpl::log (std::string msg) {
        LOG (INFO) << "Synchro @" << this << ": '" << msg << "'";
        std::cout << "Synchro @" << this << ": '" << msg << "'" << std::endl;
    }

    kj::Promise <void> SynchroImpl::connect (ConnectContext context) {
        log ("Listening");
        return kj::READY_NOW;
    }

    kj::Promise <void> SynchroImpl::updateDirection (UpdateDirectionContext context) {
        std::string msg = "Update direction to ";
        auto direction = context.getParams().getDirection();
        if (direction.getRotateLeft() && !direction.getRotateRight()) msg += '<';
        else msg += ' ';
        if (direction.getAccelerate()) msg += '^';
        else if (direction.getDecelerate()) msg += '_';
        else msg += '*';
        if (!direction.getRotateLeft() && direction.getRotateRight()) msg += '>';
        else msg += ' ';
        log (msg);
        return kj::READY_NOW;
    }
}

/* Copyright © 2022 Aaron Alef */
