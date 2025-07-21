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
        src/util/functions.cpp
        src/util/functions.h
        src/util/Haltable.cpp
        src/util/Haltable.h)

include_directories(src/)


# Libsodium
target_include_directories(MiniDB PRIVATE ${SODIUM_INCLUDE_DIRS})
target_link_libraries(MiniDB PRIVATE ${SODIUM_LIBRARIES})

# GoogleTest
include(FetchContent)
FetchContent_Declare(
     googletest
     URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)