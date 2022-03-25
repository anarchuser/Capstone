#define CATCH_CONFIG_RUNNER
#define GOOGLE_STRIP_LOG 0

#include <glog/logging.h>
#include <catch2/catch.hpp>

#include "config.h"

using namespace std;

int main (int argc, char * argv[]) {
    google::InitGoogleLogging(argv [0]);
    return Catch::Session().run (argc, argv);
}

/* Copyright (C) 2022 Aaron Alef */
