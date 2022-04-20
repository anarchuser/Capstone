#ifndef CP_HELLO_WORLD_H
#define CP_HELLO_WORLD_H

#include "config.h"

namespace hw {
    void actor_preinit ();

    void actor_init ();

    void actor_destroy ();

    void actor_update ();

    void actor_flush ();
}

#endif//CP_HELLO_WORLD_H