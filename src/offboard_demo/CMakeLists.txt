cmake_minimum_required(VERSION 3.8)
project(offboard_demo)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(px4_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(eigen3_cmake_module REQUIRED)
find_package(Eigen3 REQUIRED)


include_directories(
  include
  ${EIGEN3_INCLUDE_DIR}
)
add_executable(offboard_demo 
  OffboardDemo.cpp)

  ament_target_dependencies(offboard_demo rclcpp  geometry_msgs px4_msgs Eigen3)
  target_compile_features(offboard_demo PUBLIC c_std_99 cxx_std_17)
  
  install(TARGETS offboard_demo DESTINATION lib/${PROJECT_NAME})
  
  if(BUILD_TESTING)
      find_package(ament_lint_auto REQUIRED)
      ament_lint_auto_find_test_dependencies()
  endif()

  
  install(DIRECTORY launch
    DESTINATION share/${PROJECT_NAME}/
  )
  
  install(DIRECTORY cfg
    DESTINATION share/${PROJECT_NAME}/
  )

ament_package()