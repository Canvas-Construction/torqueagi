cmake_minimum_required(VERSION 3.5)
project(TorqueAGI)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

add_executable(torqueagi
    main.cpp
    apiclient.cpp
    visualizer.cpp
)

target_link_libraries(torqueagi
    ${OpenCV_LIBS}
    CURL::libcurl
    nlohmann_json::nlohmann_json
)
