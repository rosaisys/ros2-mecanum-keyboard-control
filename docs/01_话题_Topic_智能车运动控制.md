# ROS2话题（Topic）示例 - 智能车运动控制

## 概述
话题是ROS2中最常用的通信机制，用于发布/订阅数据流。在智能车中，话题用于：
- 发布车速指令 (`/cmd_vel`)
- 订阅传感器数据（激光雷达、IMU等）
- 发布导航目标
- 订阅电池状态等

## 1. 智能车车速发布节点

### 功能描述
- 每100ms发布一次车速指令到 `/cmd_vel` 话题
- 控制智能车前进、后退、转弯
- 使用 `geometry_msgs/Twist` 消息类型

### 代码实现 (`speed_publisher.cpp`)

```cpp
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include <chrono>

using namespace std::chrono_literals;

/**
 * @brief 智能车车速发布节点
 * 
 * 功能：定时发布车速指令到/cmd_vel话题
 * 对应智能车场景：控制电机运动
 */
class SpeedPublisher : public rclcpp::Node {
public:
    SpeedPublisher() : Node("speed_publisher") {
        // 创建发布者，话题名为/cmd_vel，队列深度10
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);
        
        // 创建定时器，每100ms发布一次车速
        timer_ = this->create_wall_timer(
            100ms,
            std::bind(&SpeedPublisher::timer_callback, this)
        );
        
        RCLCPP_INFO(this->get_logger(), "智能车车速发布节点已启动！");
    }

private:
    void timer_callback() {
        auto message = geometry_msgs::msg::Twist();
        
        // 设置车速参数 - 根据实际智能车调整
        message.linear.x = 0.5;   // 线速度：0.5 m/s（前进）
        message.linear.y = 0.0;
        message.linear.z = 0.0;
        message.angular.x = 0.0;
        message.angular.y = 0.0;
        message.angular.z = 0.2;  // 角速度：0.2 rad/s（小转弯）
        
        // 发布消息
        publisher_->publish(message);
        
        // 打印日志
        RCLCPP_INFO(this->get_logger(), 
                   "发布车速指令：线速度=%.2f m/s, 角速度=%.2f rad/s", 
                   message.linear.x, message.angular.z);
    }
    
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<SpeedPublisher>());
    rclcpp::shutdown();
    return 0;
}
```

## 2. 智能车传感器数据订阅节点

### 功能描述
- 订阅 `/sensor_data` 话题
- 接收并处理传感器数据（如激光雷达、IMU、超声波等）
- 使用 `std_msgs/Float32` 消息类型（示例）

### 代码实现 (`sensor_subscriber.cpp`)

```cpp
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float32.hpp"

/**
 * @brief 智能车传感器数据订阅节点
 * 
 * 功能：订阅传感器数据话题，接收并处理数据
 * 对应智能车场景：接收激光雷达、IMU等传感器数据
 */
class SensorSubscriber : public rclcpp::Node {
public:
    SensorSubscriber() : Node("sensor_subscriber") {
        // 创建订阅者，话题名为/sensor_data，队列深度10
        subscription_ = this->create_subscription<std_msgs::msg::Float32>(
            "/sensor_data", 10,
            std::bind(&SensorSubscriber::topic_callback, this, std::placeholders::_1)
        );
        
        RCLCPP_INFO(this->get_logger(), "智能车传感器订阅节点已启动！等待数据...");
    }

private:
    void topic_callback(const std_msgs::msg::Float32::SharedPtr msg) const {
        // 处理接收到的传感器数据
        float sensor_value = msg->data;
        
        RCLCPP_INFO(this->get_logger(), 
                   "收到传感器数据：%.2f", 
                   sensor_value);
        
        // 这里可以添加智能车特定的处理逻辑：
        // 1. 判断是否超过安全阈值
        if (sensor_value > 50.0) {
            RCLCPP_WARN(this->get_logger(), "传感器数据超过阈值：%.2f", sensor_value);
        }
        
        // 2. 触发相应控制逻辑
        // 3. 记录数据到日志文件
        // 4. 发送到其他节点进行进一步处理
    }
    
    rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr subscription_;
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<SensorSubscriber>());
    rclcpp::shutdown();
    return 0;
}
```

## 3. 智能车电池状态监控节点

### 功能描述
- 订阅 `/battery_status` 话题
- 监控电池电压和电量
- 低电量时发出警告

### 代码实现 (`battery_monitor.cpp`)

```cpp
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float32.hpp"

/**
 * @brief 智能车电池状态监控节点
 * 
 * 功能：监控电池电量，低电量时发出警告
 * 对应智能车场景：电池管理，防止突然断电
 */
class BatteryMonitor : public rclcpp::Node {
public:
    BatteryMonitor() : Node("battery_monitor") {
        // 创建订阅者，话题名为/battery_voltage
        subscription_ = this->create_subscription<std_msgs::msg::Float32>(
            "/battery_voltage", 10,
            std::bind(&BatteryMonitor::battery_callback, this, std::placeholders::_1)
        );
        
        RCLCPP_INFO(this->get_logger(), "智能车电池监控节点已启动！");
    }

private:
    void battery_callback(const std_msgs::msg::Float32::SharedPtr msg) const {
        float voltage = msg->data;
        
        // 根据电压判断电池状态
        if (voltage > 12.6) {
            RCLCPP_INFO(this->get_logger(), "电池电量充足：%.2fV", voltage);
        } else if (voltage > 11.5) {
            RCLCPP_WARN(this->get_logger(), "电池电量中等：%.2fV，建议充电", voltage);
        } else {
            RCLCPP_ERROR(this->get_logger(), "电池电量低：%.2fV，请立即充电！", voltage);
        }
    }
    
    rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr subscription_;
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<BatteryMonitor>());
    rclcpp::shutdown();
    return 0;
}
```

## 4. 编译配置

### CMakeLists.txt 配置
```cmake
# 添加话题发布者可执行文件
add_executable(speed_publisher src/speed_publisher.cpp)
ament_target_dependencies(speed_publisher rclcpp geometry_msgs)

# 添加传感器订阅者可执行文件
add_executable(sensor_subscriber src/sensor_subscriber.cpp)
ament_target_dependencies(sensor_subscriber rclcpp std_msgs)

# 添加电池监控可执行文件
add_executable(battery_monitor src/battery_monitor.cpp)
ament_target_dependencies(battery_monitor rclcpp std_msgs)

# 安装可执行文件
install(TARGETS speed_publisher sensor_subscriber battery_monitor
  DESTINATION lib/${PROJECT_NAME}
)
```

### package.xml 依赖
```xml
<depend>rclcpp</depend>
<depend>geometry_msgs</depend>
<depend>std_msgs</depend>
```

## 5. 运行测试

### 测试步骤
1. **编译包**：
   ```bash
   cd ~/ros2_ws
   colcon build --packages-select smart_car_core_demo
   source install/setup.bash
   ```

2. **运行车速发布节点**：
   ```bash
   ros2 run smart_car_core_demo speed_publisher
   ```
   输出：每100ms打印一次发布的车速指令

3. **模拟传感器数据**（新终端）：
   ```bash
   ros2 topic pub /sensor_data std_msgs/msg/Float32 "{data: 25.5}" -r 1
   ```

4. **运行传感器订阅节点**（新终端）：
   ```bash
   ros2 run smart_car_core_demo sensor_subscriber
   ```
   输出：显示接收到的传感器数据

5. **查看话题列表**：
   ```bash
   ros2 topic list
   ```

6. **查看话题数据**：
   ```bash
   ros2 topic echo /cmd_vel
   ```

## 6. 实际智能车应用建议

### 话题命名规范
- 控制话题：`/cmd_vel`（标准ROS话题）
- 传感器话题：`/sensor/[类型]`，如 `/sensor/lidar`、`/sensor/imu`
- 状态话题：`/status/[类型]`，如 `/status/battery`、`/status/motor`

### QoS设置
对于智能车控制，建议使用可靠的QoS：
```cpp
auto qos = rclcpp::QoS(rclcpp::KeepLast(10)).reliable();
publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", qos);
```

### 性能优化
1. **发布频率**：控制指令建议10-100Hz
2. **队列深度**：根据网络延迟调整，通常10-20
3. **消息大小**：尽量使用紧凑的消息格式

## 总结
话题是智能车ROS2系统的核心通信机制。通过本示例，您可以：
1. 掌握话题发布/订阅的基本模式
2. 了解智能车常用的话题类型
3. 学会如何在实际智能车上应用话题通信
4. 为后续的服务和动作学习打下基础

**下一步**：尝试修改车速参数，观察智能车实际响应，加深对话题通信的理解。
