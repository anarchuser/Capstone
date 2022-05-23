#include "Address.h"

namespace cg {
    Address::Address (Backend::Address::Reader reader): Address (reader.getIp(), reader.getPort()) {}
    Address::Address (std::string && ip, unsigned short port)
            : ip {std::move (ip)}
            , port {port}
            {
            }

    void Address::initialise (Backend::Address::Builder builder) const {
        builder.setIp (ip);
        builder.setPort (port);
    }

    Address::operator std::string () const {
        return ip + ":" + std::to_string (port);
    }
}

/* Copyright © 2022 Aaron Alef */
