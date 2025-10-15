cmake_minimum_required(VERSION 3.8)
project(precision_land_executor)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(px4_ros2_cpp REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2 REQUIRED)

# Executable target
add_executable(${PROJECT_NAME}
  CustomMode_v2.cpp
  CustomMode_v2.hpp
  CustomModeExecutor_v2.cpp
  CustomModeExecutor_v2.hpp
)

ament_target_dependencies(${PROJECT_NAME}
  rclcpp
  px4_ros2_cpp
  Eigen3
  geometry_msgs
  tf2_ros
  tf2
)

# Install the binary
install(TARGETS ${PROJECT_NAME}
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)

install(DIRECTORY cfg
  DESTINATION share/${PROJECT_NAME}/
)

# Linting
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_cpplint_FOUND TRUE)
  set(ament_cmake_copyright_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()