cmake_minimum_required(VERSION 3.5)
project(px4_gimbal_control)

set(CMAKE_CXX_STANDARD 20)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(px4_msgs REQUIRED)

add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter)

include_directories(include)

add_executable(gimbal_teleop src/GimbalTeleop.cpp)

ament_target_dependencies(gimbal_teleop
  rclcpp
  geometry_msgs
  px4_msgs
)

add_library(mouse_pointer_gimbal_control_lib STATIC
  src/mouse_pointer_gimbal_control.cpp
)

ament_target_dependencies(mouse_pointer_gimbal_control_lib
  rclcpp
  geometry_msgs
  sensor_msgs
)

add_executable(mouse_pointer_gimbal_control_solution src/solution.cpp)
add_executable(mouse_pointer_gimbal_control_exercise src/exercise.cpp)

target_link_libraries(mouse_pointer_gimbal_control_solution
  mouse_pointer_gimbal_control_lib
)

target_link_libraries(mouse_pointer_gimbal_control_exercise
  mouse_pointer_gimbal_control_lib
)

ament_target_dependencies(mouse_pointer_gimbal_control_solution
  rclcpp
  geometry_msgs
  sensor_msgs
)

ament_target_dependencies(mouse_pointer_gimbal_control_exercise
  rclcpp
  geometry_msgs
  sensor_msgs
)

install(TARGETS gimbal_teleop mouse_pointer_gimbal_control_solution
                mouse_pointer_gimbal_control_exercise
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY launch cfg
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
    find_package(ament_lint_auto REQUIRED)
    ament_lint_auto_find_test_dependencies()
endif()

ament_package()