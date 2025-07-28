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
        src/core/storage/page/Page.cpp
        src/core/storage/page/Page.h
        src/core/storage/page/BufferPool.cpp
        src/core/storage/page/BufferPool.h
        src/core/storage/file/IndexFile.cpp
        src/core/storage/file/IndexFile.h
        src/core/storage/file/DataFile.cpp
        src/core/storage/file/DataFile.h
        src/core/util/constants.h
        src/core/storage/page/SlottedPage.cpp
        src/core/storage/page/SlottedPage.h
        src/core/storage/page/IndexHeaderPage.cpp
        src/core/storage/page/IndexHeaderPage.h
        src/core/storage/page/IndexPartitionHeaderPage.cpp
        src/core/storage/page/IndexPartitionHeaderPage.h
        src/core/storage/page/DataHeaderPage.cpp
        src/core/storage/page/DataHeaderPage.h
        src/core/storage/page/DataPartitionHeaderPage.cpp
        src/core/storage/page/DataPartitionHeaderPage.h
        src/core/storage/pointer/Pointer.cpp
        src/core/storage/pointer/Pointer.h
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