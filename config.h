#ifndef Capstone_CONFIG_H
#define Capstone_CONFIG_H

#include <filesystem>
namespace fs = std::filesystem;
fs::path const DATA_DIR = PROJECT_ROOT "/data";

#define USERNAME "azazel"
#define SERVER_ADDRESS "*"
#define REMOTE_ADDRESS "192.168.178.35"
#define SERVER_PORT    44444
#define SERVER_FULL_ADDRESS (std::string (SERVER_ADDRESS)  + ":" + std::to_string (SERVER_PORT))

static std::string remote = REMOTE_ADDRESS;

#endif //Capstone_CONFIG_H

/* Copyright (C) 2022 Aaron Alef */
