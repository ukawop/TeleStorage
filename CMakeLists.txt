cmake_minimum_required(VERSION 3.10)
project(TeleStorage)

set(CMAKE_CXX_STANDARD 17)

include_directories(
    include
    libs/include
)

set(CMAKE_FIND_LIBRARY_SUFFIXES ".so")

find_library(NCURSES_LIBRARIES NAMES ncurses form REQUIRED)
find_library(SQLITE_LIBRARIES NAMES sqlite3 REQUIRED)
find_library(SSL_LIBRARIES NAMES ssl REQUIRED PATHS /usr/lib/x86_64-linux-gnu)
find_library(CRYPTO_LIBRARIES NAMES crypto REQUIRED PATHS /usr/lib/x86_64-linux-gnu)

file(GLOB SOURCES "src/*.cpp")

add_executable(TeleStorage ${SOURCES})

target_link_libraries(TeleStorage
    ${CMAKE_SOURCE_DIR}/libs/libTgBot.a
    ${NCURSES_LIBRARIES}
    ${SQLITE_LIBRARIES}
    ${SSL_LIBRARIES}
    ${CRYPTO_LIBRARIES}
)
