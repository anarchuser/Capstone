#ifndef Capstone_CONFIG_H
#define Capstone_CONFIG_H

#include <string>

/// Resource folder. Specifically, points to the resource XML file containing the relevant information
#define GAME_RESOURCES  (PROJECT_ROOT "/src/data/res.xml")

#define USERNAME        "lucifer"
#define SERVER_ADDRESS  "*"
#define REMOTE_ADDRESS  "beelzebub"
#define SERVER_PORT     44444
#define SERVER_FULL_ADDRESS (std::string (SERVER_ADDRESS)  + ":" + std::to_string (SERVER_PORT))

static std::string remote = REMOTE_ADDRESS;

#endif //Capstone_CONFIG_H

/* Copyright (C) 2022 Aaron Alef */
