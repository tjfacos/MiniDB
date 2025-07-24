cmake_minimum_required(VERSION 3.31)
project(MiniDB)

set(CMAKE_CXX_STANDARD 20)

add_executable(MiniDB src/main.cpp
        src/server/Server.cpp
        src/server/Server.h
        src/common/Connection.cpp
        src/common/Connection.h
        src/server/ConnectionManager.cpp
        src/server/ConnectionManager.h
        src/common/util/logging.cpp
        src/common/util/logging.h
        src/common/Haltable.cpp
        src/common/Haltable.h
        src/client/Client.cpp
        src/client/Client.h
        src/common/util/config.cpp
        src/common/util/config.h
        src/common/messaging/networking.cpp
        src/common/messaging/networking.h
)

include_directories(src/)

# Libsodium
target_link_libraries(MiniDB PRIVATE sodium)
