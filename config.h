#ifndef Capstone_CONFIG_H
#define Capstone_CONFIG_H

// TODO: remove
#include <iostream>

/** Google Logging Library */
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

#endif //Capstone_CONFIG_H

/* Copyright (C) 2022 Aaron Alef */
