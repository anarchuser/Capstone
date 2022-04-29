#include "Server.h"

namespace cg {

    void SynchroImpl::log (std::string msg) {
        LOG (INFO) << "Synchro @" << this << ": '" << msg << "'";
    }

    kj::Promise <void> SynchroImpl::connect (ConnectContext context) {
        log ("Listening");
        return kj::READY_NOW;
    }

    kj::Promise <void> SynchroImpl::updateDirection (UpdateDirectionContext context) {
        std::string msg = "Update direction to ";
        auto dir = context.getParams().getDirection();
        if (dir.getAccelerate()) msg += 'W';
        else if (dir.getDecelerate()) msg += 'S';
        if (dir.getRotateLeft() && !dir.getRotateRight()) msg += 'A';
        if (dir.getRotateRight() && !dir.getRotateLeft()) msg += 'D';
        log (msg);
        return kj::READY_NOW;
    }
}

/* Copyright © 2022 Aaron Alef */
