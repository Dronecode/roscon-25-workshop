cmake_minimum_required(VERSION 3.8)
project(sar_modes)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
  SARMode.hpp
  SARMode.cpp
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

# Install the fake rover pose publisher used for testing without a rover model
install(PROGRAMS
  scripts/fake_rover_mover.py
  DESTINATION lib/${PROJECT_NAME}
)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/launch)
  install(DIRECTORY launch
    DESTINATION share/${PROJECT_NAME}
  )
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cfg)
  install(DIRECTORY cfg
    DESTINATION share/${PROJECT_NAME}/
  )
endif()

# Linting
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_cpplint_FOUND TRUE)
  set(ament_cmake_copyright_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
