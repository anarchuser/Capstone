#ifndef Capstone_CONFIG_H
#define Capstone_CONFIG_H

/// Google Logging Library
// Minimum LOG level. 0 = Everything, 1 = Ignore LOG (INFO), ...
#define GOOGLE_STRIP_LOG 0
#include <glog/logging.h>

#ifdef ERRORS_AS_WARNINGS
#define THROW(error) {              \
    LOG (WARNING) << error.what();  \
    throw error;                    \
}; true
#else
#define THROW(error) {              \
    LOG (ERROR) << error.what();    \
    throw error;                    \
}; true
#endif

#include <filesystem>
namespace fs = std::filesystem;
fs::path const DATA_DIR = PROJECT_ROOT "/data";

#define SERVER_ADDRESS "192.168.178.36"
#define REMOTE_ADDRESS "192.168.178.35"
#define SERVER_PORT    44444
#define SERVER_FULL_ADDRESS (std::string (SERVER_ADDRESS)  + ":" + std::to_string (SERVER_PORT))

static std::string remote = REMOTE_ADDRESS;

#endif //Capstone_CONFIG_H

/* Copyright (C) 2022 Aaron Alef */
