cmake_minimum_required(VERSION 3.22.1)
project(soccerRobotFramework)
set(CMAKE_CXX_STANDARD 20)
add_library(soccerRobotFramework STATIC
        src/agent.cpp
        src/messaging/mailbox.cpp
        src/messaging/event.h
        src/messaging/mailbox.h
        src/messaging/priority.cpp
        src/messaging/priority.h
        src/types.h
)
add_executable(main main.cpp)

target_link_libraries(main PRIVATE soccerRobotFramework)

find_package(cppzmq CONFIG REQUIRED)
target_link_libraries(main PRIVATE cppzmq cppzmq-static)