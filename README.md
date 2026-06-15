# ROS2 麦伦麦克纳姆轮智能车键盘控制

基于 ROS2 Humble 的麦伦麦克纳姆轮智能车键盘遥操作控制项目，支持跨平台键盘控制、麦克纳姆轮逆运动学解算、加速度限幅平滑控制、多模式切换等功能。

**硬件平台**：麦伦麦克纳姆轮底盘 + G4激光雷达 + 奥比中光Pro深度相机 + STM32下位机

## 目录结构

```
├── README.md                         # 本文件
├── docs/                             # 文档目录
├── src/
│   ├── driver/                       # 驱动层
│   │   ├── ros_robot_controller/     # 机器人控制器（厂家驱动）
│   │   ├── ros_robot_controller_msgs/# 机器人控制器消息定义
│   │   └── speed_control_msgs/       # 速度控制自定义服务消息
│   └── peripherals/                  # 应用层（smart_car_core_demo）
│       ├── config/params.yaml        # 全局参数配置文件
│       ├── launch/topic_demo.launch.py # 启动文件
│       └── src/
│           ├── speed_publisher.cpp       # 键盘遥操作速度发布节点
│           ├── speed_subscriber.cpp      # 速度订阅+运动学解算节点
│           ├── control_mode_client.cpp   # 控制模式切换节点
│           └── keyboard_input.cpp        # 键盘输入节点
├── build/                            # 构建输出目录
├── install/                          # 安装目录
└── log/                              # 日志目录
```

## 包含的包

### 1. smart_car_core_demo （`src/peripherals/`）
智能车核心演示包，包含以下节点：

| 节点 | 文件 | 功能 |
|------|------|------|
| `speed_publisher` | `speed_publisher.cpp` | 跨平台键盘遥操作，加速度限幅平滑控制，100Hz高频发布，ROS2参数全配置化 |
| `speed_subscriber` | `speed_subscriber.cpp` | 基于麦克纳姆轮逆运动学解算Twist指令为四轮独立转速，支持运行时参数重载，集成紧急停止保护 |
| `control_mode_client` | `control_mode_client.cpp` | 通过ROS2 Service动态切换平滑/竞速双模式 |
| `keyboard_input` | `keyboard_input.cpp` | 跨平台非阻塞键盘检测输入节点 |

### 2. ros_robot_controller_msgs
机器人控制器消息定义包，定义与STM32下位机通信的消息格式（电机控制、舵机、总线舵机、OLED、蜂鸣器等）。

### 3. speed_control_msgs
速度控制自定义服务消息，提供速度调整的Service定义。

### 4. ros_robot_controller
厂家提供的机器人控制器驱动包，负责通过串口与STM32下位机通信，执行电机控制指令。

## 环境要求

- **OS**: Ubuntu 22.04 LTS
- **ROS2**: Humble Hawksbill
- **编译器**: gcc/g++ 11+
- **构建工具**: CMake 3.16+, colcon
- **Python**: 3.10+
- **依赖**: colcon-common-extensions

## 编译方法

### 1. 安装依赖

```bash
# 确保已安装 ROS2 Humble
sudo apt update
sudo apt install -y ros-humble-desktop python3-colcon-common-extensions

# 确保 ROS2 环境已配置
echo "source /opt/ros/humble/setup.bash" >> ~/.bashrc
source ~/.bashrc
```

### 2. 克隆并编译

```bash
# 克隆仓库
git clone https://github.com/rosaisys/ros2-mecanum-keyboard-control.git
cd ros2-mecanum-keyboard-control

# 方法一：编译所有包
colcon build

# 方法二：仅编译特定包（推荐首次编译）
colcon build --packages-select ros_robot_controller_msgs speed_control_msgs smart_car_core_demo

# 方法三：使用并行编译加速
colcon build --parallel-workers 4

# 方法四：如果遇到CMake缓存路径错误，先清理再编译
rm -rf build/ install/ log/
colcon build
```

### 3. 加载环境

```bash
# 每次打开新终端时都需要执行
source install/setup.bash
# 或写入bashrc
echo "source $(pwd)/install/setup.bash" >> ~/.bashrc
```

## 运行方法

### 基础运行（仅速度控制）

```bash
# 终端1：运行机器人控制器（与硬件通信）
ros2 run ros_robot_controller ros_robot_controller

# 终端2：运行速度订阅者（运动学解算）
ros2 run smart_car_core_demo speed_subscriber

# 终端3：运行键盘速度发布者
ros2 run smart_car_core_demo speed_publisher
```

### 控制模式切换

```bash
# 终端4：运行控制模式切换客户端
ros2 run smart_car_core_demo control_mode_client
```
- 按 `s` 切换到**平滑模式**（平稳起步，加速度限幅）
- 按 `o` 切换到**原始竞速模式**（直接输出键盘速度）
- 按 `q` 退出

### 使用Launch文件一键启动

```bash
ros2 launch smart_car_core_demo topic_demo.launch.py
```

## 系统架构

```
[键盘] → speed_publisher → (cmd_vel话题) → speed_subscriber
                                              ↓
                                    [麦克纳姆轮逆运动学]
                                              ↓
                              (ros_robot_controller/set_motor话题)
                                              ↓
                              ros_robot_controller → (串口/dev/rrc)
                                              ↓
                                      [STM32电机执行]

控制模式切换：
control_mode_client ──→ (set_control_mode Service) ──→ speed_publisher
    s: 平滑模式（加速度限幅）       ↑
    o: 原始竞速模式（直接输出）     speed_control_msgs/AdjustSpeed Service
```

## 参数配置

所有参数通过 `src/peripherals/config/params.yaml` 集中管理：

| 参数 | 默认值 | 说明 |
|------|--------|------|
| `speed_publisher.default_linear_velocity` | 0.2 | 默认线速度 (m/s) |
| `speed_publisher.default_angular_velocity` | 0.5 | 默认角速度 (rad/s) |
| `speed_publisher.acceleration_limit` | 2.5 | 线加速度限制 (m/s²) |
| `speed_publisher.publish_rate` | 100.0 | 发布频率 (Hz) |
| `speed_subscriber.wheelbase` | 0.216 | 前后轴距 (m) |
| `speed_subscriber.track_width` | 0.195 | 左右轮距 (m) |
| `speed_subscriber.wheel_diameter` | 0.097 | 轮子直径 (m) |
| `control_mode_client.service_name` | set_control_mode | 控制模式服务名 |

## 维护

### 清理构建文件
```bash
rm -rf build/ install/ log/
```

### 重新编译某包
```bash
colcon build --packages-select smart_car_core_demo --cmake-clean-first
```

### 常见问题
1. **CMake缓存路径错误**：`rm -rf build/ install/ log/` 后重新编译
2. **找不到包**：确保已执行 `source install/setup.bash`
3. **串口权限**：确保用户有 `/dev/rrc` 或 `/dev/ttyACM0` 的访问权限
4. **键盘输入无效**：确保终端焦点在运行 `speed_publisher` 的窗口

## 文档

详细说明请查看 `docs/` 目录：
- [如何运行智能车速度控制节点](docs/如何运行智能车速度控制节点.md)
- [重要开发文档](docs/Important%20secondary%20development%20documentation.md)

## 许可证

本项目基于MIT许可证。
