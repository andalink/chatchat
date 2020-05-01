cmake_minimum_required(VERSION 3.5)

project(chatchat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(SERVER_SOURCES
        chat_srv.cpp
        server.cpp
        client_handler.cpp
    )

set(SERVER_HEADERS
        server.h
        client_handler.h
        common.h
    )

add_executable(chatchatd ${SERVER_SOURCES} ${SERVER_HEADERS})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(chatchatd LINK_PUBLIC ${CMAKE_THREAD_LIBS_INIT})