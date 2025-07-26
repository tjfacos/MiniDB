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

# Core Library
add_library(MiniDB_core STATIC
        src/core/storage/Page.cpp
        src/core/storage/Page.h
        src/core/util/constants.h
)

# Database application
add_executable(MiniDB_server
        src/server/main.cpp
        src/server/Server.h
        src/server/Server.cpp
        src/server/ConnectionManager.h
        src/server/ConnectionManager.cpp
)

target_link_libraries(MiniDB_server PRIVATE MiniDB_common sodium)

# Client application
add_executable(MiniDB_client
        src/client/main.cpp
        src/client/Client.h
        src/client/Client.cpp
)

target_link_libraries(MiniDB_client PRIVATE MiniDB_common sodium)

set_target_properties(MiniDB_server PROPERTIES OUTPUT_NAME "MiniDB_server")
set_target_properties(MiniDB_client PROPERTIES OUTPUT_NAME "MiniDB_client")
set_target_properties(MiniDB_common PROPERTIES OUTPUT_NAME "MiniDB_common")