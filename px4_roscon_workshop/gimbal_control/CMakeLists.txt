cmake_minimum_required(VERSION 3.5)
project(px4_gimbal_control)

set(CMAKE_CXX_STANDARD 20)

find_package(ament_cmake REQUIRED)

add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter)

install(DIRECTORY launch cfg
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
    find_package(ament_lint_auto REQUIRED)
    ament_lint_auto_find_test_dependencies()
endif()

ament_package()