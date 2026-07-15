cmake_minimum_required(VERSION 3.5)
project(px4_formation_control)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(eigen3_cmake_module REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};/usr/share/cmake/geographiclib")
find_package(GeographicLib REQUIRED)

find_package(px4_ros2_cpp REQUIRED)


include_directories(
    include
    ${Eigen3_INCLUDE_DIRS}
    ${GeographicLib_INCLUDE_DIRS}
)

add_executable(px4_formation_control
    src/main.cpp
)

ament_target_dependencies(px4_formation_control
    rclcpp
    Eigen3
    px4_ros2_cpp
    geometry_msgs
    tf2
    tf2_ros
)

target_link_libraries(px4_formation_control
    ${GeographicLib_LIBRARIES}
)

target_compile_features(px4_formation_control PUBLIC c_std_99 cxx_std_20)

install(
    TARGETS px4_formation_control
    DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
    find_package(ament_lint_auto REQUIRED)
    ament_lint_auto_find_test_dependencies()
endif()

ament_package()