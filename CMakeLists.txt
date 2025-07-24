cmake_minimum_required(VERSION 3.31)
project(MiniDB)

set(CMAKE_CXX_STANDARD 20)

include_directories(src/)

# Common Library
add_library(MiniDB_common STATIC
        src/common/Connection.h
        src/common/Connection.cpp
        src/common/Haltable.h
        src/common/Haltable.cpp
        src/common/util/config.h
        src/common/util/config.cpp
        src/common/util/logging.h
        src/common/util/logging.cpp
        src/common/util/networking.h
        src/common/util/networking.cpp
        src/common/constants.h
)

target_link_libraries(MiniDB_common PRIVATE sodium)

# Database application
add_executable(MiniDB_server
        src/server/main.cpp
        src/server/Server.h
        src/server/Server.cpp
)

target_link_libraries(MiniDB_server PRIVATE MINIDB_common sodium)

# Client application
add_executable(MiniDB_client
        src/client/main.cpp
        src/client/Client.h
        src/client/Client.cpp
)

target_link_libraries(MiniDB_client PRIVATE MINIDB_common sodium)