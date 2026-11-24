cmake_minimum_required(VERSION 3.8.1)
project(cpp_tcp)

find_package(Threads)
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++23")

add_library(${PROJECT_NAME}
src/tcp_server.cpp
src/pipe_ret_t.cpp
src/tcp_client.cpp
src/common.cpp
src/epoll.cpp
src/client.cpp)


option(SERVER_EXAMPLE "Build SERVER" ON)
option(SAMPLE "Enable the sample feature for demonstration purposes" ON)
option(CLIENT_EXAMPLE "Build Client" ON)


if(SERVER_EXAMPLE)
        add_definitions(
                -DSERVER_EXAMPLE
        )

        add_executable(tcp_server examples/server_example.cpp)

        target_link_libraries(tcp_server ${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
endif()

if(SAMPLE)
        file(GLOB_RECURSE mains CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/examples/*.cpp)
        foreach(mainfile IN LISTS mains)
                get_filename_component(mainname ${mainfile} NAME_WE)
                add_executable(${mainname} ${mainfile})
                 target_link_libraries(${mainname} PRIVATE ${PROJECT_NAME})
              endforeach(mainfile IN LISTS mains)
endif()

if(CLIENT_EXAMPLE)
    add_definitions(
            -DCLIENT_EXAMPLE
    )
    add_executable(tcp_client examples/client_example.cpp)
    target_link_libraries (tcp_client ${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
endif()
