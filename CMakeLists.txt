cmake_minimum_required(VERSION 3.22.1)
project(soccerRobotFramework)
set(CMAKE_CXX_STANDARD 20)
add_library(soccerRobotFramework STATIC
        src/types.h
        src/agent/agent.cpp
        src/agent/agent.h
        src/messaging/event.h
        src/messaging/mailbox/mailbox.cpp
        src/messaging/mailbox/mailbox.h
        src/messaging/priority/priority.cpp
        src/messaging/priority/priority.h
)
find_package(ZeroMQ CONFIG REQUIRED)
target_link_libraries(soccerRobotFramework PUBLIC libzmq libzmq-static)
find_package(cppzmq CONFIG REQUIRED)
target_link_libraries(soccerRobotFramework PUBLIC cppzmq cppzmq-static)
find_package(rerun_sdk CONFIG REQUIRED)
target_link_libraries(soccerRobotFramework PUBLIC rerun_sdk)
find_package (Boost REQUIRED COMPONENTS headers thread)
target_link_libraries(soccerRobotFramework PUBLIC Boost::headers Boost::thread)

# Run the framework!
add_executable(main main.cpp)
target_link_libraries(main PRIVATE soccerRobotFramework)

# Test the framework!
add_executable(test test.cpp)
target_link_libraries(test PRIVATE soccerRobotFramework)
find_package(doctest CONFIG REQUIRED)
target_link_libraries(test PRIVATE doctest::doctest)
