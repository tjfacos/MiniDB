cmake_minimum_required(VERSION 3.31)
project(MiniDB)

set(CMAKE_CXX_STANDARD 20)

add_executable(MiniDB src/main.cpp
        src/server/Server.cpp
        src/server/Server.h
        src/server/Connection.cpp
        src/server/Connection.h
        src/server/ConnectionManager.cpp
        src/server/ConnectionManager.h
        src/util/logging.cpp
        src/util/logging.h
        src/util/Haltable.cpp
        src/util/Haltable.h
        src/client/Client.cpp
        src/client/Client.h
        src/util/config.cpp
        src/util/config.h
        src/util/networking.cpp
        src/util/networking.h
)

include_directories(src/)

# Libsodium
target_link_libraries(MiniDB PRIVATE sodium)
