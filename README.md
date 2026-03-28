# ROS2 智能车控制工作空间

这是一个用于智能车控制的ROS2工作空间，包含核心演示包和机器人控制器。

## 目录结构

```
ros2_wsorincp/
├── README.md                    # 本文件
├── docs/                        # 文档目录
│   ├── Important secondary development documentation.md
│   ├── 如何运行智能车速度控制节点.md
│   └── ... (其他文档文件)
├── scripts/                     # 脚本目录
│   └── setup_workspace.zsh      # 工作空间环境设置脚本
├── ros2_core_examples/          # 核心示例代码
│   ├── README.md
│   ├── docs/                    # 示例文档
│   ├── smart_car_core_demo/     # 智能车核心演示包
│   └── src/                     # 源代码
│       ├── ros_robot_controller/        # 机器人控制器
│       └── ros_robot_controller_msgs/   # 消息定义
├── build/                       # 构建输出目录
├── install/                     # 安装目录
└── log/                         # 日志目录
```

## 快速开始

### 1. 设置环境

```bash
# 进入工作空间目录
cd ~/ros2_wsorincp

# 设置ROS2环境
source /opt/ros/humble/setup.bash

# 设置工作空间环境
source install/setup.bash

# 或者使用提供的脚本（针对zsh用户）
source scripts/setup_workspace.zsh
```

### 2. 编译工作空间

```bash
# 编译所有包
colcon build

# 编译特定包
colcon build --packages-select smart_car_core_demo ros_robot_controller_msgs ros_robot_controller

# 使用并行编译加速
colcon build --parallel-workers 4
```

### 3. 运行智能车速度控制节点

```bash
# 终端1：运行机器人控制器
ros2 run ros_robot_controller ros_robot_controller

# 终端2：运行速度订阅者
ros2 run smart_car_core_demo speed_subscriber

# 终端3：运行速度发布者
ros2 run smart_car_core_demo speed_publisher
```

## 包含的包

### 1. smart_car_core_demo
智能车核心演示包，包含：
- `speed_publisher`: 发布速度指令到`cmd_vel`话题
- `speed_subscriber`: 订阅`cmd_vel`话题，计算电机转速

### 2. ros_robot_controller_msgs
机器人控制器消息定义包，定义与硬件通信的消息格式。

### 3. ros_robot_controller
机器人控制器包，负责：
- 订阅电机控制指令
- 通过串口与硬件通信
- 控制智能车运动

## 系统架构

```
speed_publisher → (cmd_vel话题) → speed_subscriber → 
(ros_robot_controller/set_motor话题) → ros_robot_controller → 
(串口/dev/rrc) → 电机控制器 → 电机
```

## 文档

详细的使用说明和故障排除指南请查看`docs/`目录：
- [如何运行智能车速度控制节点](docs/如何运行智能车速度控制节点.md)
- [重要开发文档](docs/Important%20secondary%20development%20documentation.md)

## 环境要求

- Ubuntu 22.04 LTS
- ROS2 Humble Hawksbill
- Python 3.10+
- CMake 3.16+

## 维护

### 清理构建文件
```bash
rm -rf build/ install/ log/
```

### 更新环境
如果遇到环境问题，请检查：
1. ROS2环境是否正确设置：`source /opt/ros/humble/setup.bash`
2. 工作空间环境是否正确设置：`source install/setup.bash`
3. 串口设备权限：确保用户有`/dev/rrc`或`/dev/ttyACM0`的访问权限

## 许可证

本项目基于MIT许可证。详见各包的许可证文件。

## 贡献

欢迎提交问题和拉取请求。请确保代码符合ROS2编码标准。
