#include "Client.h"

namespace cg {
    Client::Client (Registrar_t && registrar, kj::Maybe<Synchro_t> && synchro, Type type)
            : registrar {registrar}, synchro {synchro}, type {type} {}

    Client::Client (Registrar_t && registrar)
            : Client (std::forward<Registrar_t> (registrar), {}, LOCAL) {}
}

/* Copyright © 2022 Aaron Alef */
