cmake_minimum_required(VERSION 3.8)
project(smart_car_core_demo)

# 设置C++标准为C++17（现代C++特性支持）
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 编译器警告设置（提高代码质量）
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
  # 添加更多现代C++警告
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wdocumentation -Wdeprecated-declarations)
  endif()
endif()

# 查找ROS2依赖包
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(std_srvs REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(ros_robot_controller_msgs REQUIRED)
find_package(speed_control_msgs REQUIRED)

# 🔥【核心修复】先定义键盘输入库！！！放在最前面
add_library(keyboard_input SHARED src/keyboard_input.cpp)
ament_target_dependencies(
  keyboard_input
  rclcpp
)

# 测试配置（如果启用测试）
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

# 添加头文件包含路径
include_directories(
  include
  ${ros_robot_controller_msgs_INCLUDE_DIRS}
  ${speed_control_msgs_INCLUDE_DIRS}
)

# 🔥 库定义完了，再定义可执行文件
add_executable(speed_publisher src/speed_publisher.cpp)
ament_target_dependencies(
  speed_publisher
  rclcpp
  geometry_msgs
  std_srvs
  speed_control_msgs
)
target_link_libraries(speed_publisher keyboard_input)

add_executable(control_mode_client src/control_mode_client.cpp)
ament_target_dependencies(
  control_mode_client
  rclcpp
  std_srvs
  speed_control_msgs
)
target_link_libraries(control_mode_client keyboard_input)

add_executable(speed_subscriber src/speed_subscriber.cpp)
ament_target_dependencies(
  speed_subscriber
  rclcpp
  geometry_msgs
  std_msgs
  ros_robot_controller_msgs
)

# 安装可执行文件和库
install(TARGETS
  speed_publisher
  control_mode_client
  speed_subscriber
  keyboard_input
  DESTINATION lib/${PROJECT_NAME}
)

# 安装launch文件
install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}
)

# 安装配置文件
install(DIRECTORY
  config
  DESTINATION share/${PROJECT_NAME}
)

# 安装头文件（如果有）
install(DIRECTORY
  include/
  DESTINATION include/${PROJECT_NAME}
)

# 导出依赖
ament_export_dependencies(
  rclcpp
  geometry_msgs
  std_msgs
  std_srvs
  rclcpp_action
  ros_robot_controller_msgs
  speed_control_msgs
)

# 导出包含目录
ament_export_include_directories(include)

# 导出库
ament_export_libraries(keyboard_input)

ament_package()