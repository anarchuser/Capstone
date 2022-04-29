#include "config.h"

#include "Server/generated/synchro.capnp.h"
#include <capnp/ez-rpc.h>
#include <kj/debug.h>

#include <iostream>

int main (int argc, char * argv[]) {
    google::InitGoogleLogging (argv[0]);

    std::cout << "Simple client for testing RpcServer connections" << std::endl;


}

/* Copyright © 2022 Aaron Alef */
