# 智能车ROS2四大核心代码示例

## 概述
本文件提供智能车场景下的ROS2四大核心机制（节点/话题/服务/动作）的完整代码示例，帮助你快速找回记忆。所有代码都针对智能车实际应用场景设计，可以直接编译运行。

## 项目结构
```
src/Myself_WS/ros2_core_examples/
├── 智能车ROS2四大核心代码示例.md (本文件)
└── src/
    └── smart_car_core_demo/
        ├── CMakeLists.txt
        ├── package.xml
        ├── include/
        ├── src/
        │   ├── topic_publisher.cpp      # 话题发布者：发布车速指令
        │   ├── topic_subscriber.cpp     # 话题订阅者：订阅传感器数据
        │   ├── service_server.cpp       # 服务端：开关灯控制
        │   ├── service_client.cpp       # 客户端：发送开关灯请求
        │   ├── action_server.cpp        # 动作服务端：倒计时停车
        │   └── integrated_node.cpp      # 整合节点：车速发布+灯光控制
        └── action/
            └── Countdown.action         # 动作定义文件
```

## 1. 创建功能包
首先创建一个新的ROS2功能包：

```bash
cd ~/ros2_ws/src/Myself_WS
ros2 pkg create --build-type ament_cmake smart_car_core_demo --dependencies rclcpp geometry_msgs std_msgs std_srvs rclcpp_action
```

## 2. 话题（Topic）示例

### 2.1 话题发布者：发布车速指令 (`topic_publisher.cpp`)
```cpp
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include <chrono>

using namespace std::chrono_literals;

/**
 * @brief 智能车车速发布节点
 * 
 * 功能：每100ms发布一次车速指令到/cmd_vel话题
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
        
        // 设置车速参数
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

### 2.2 话题订阅者：订阅传感器数据 (`topic_subscriber.cpp`)
```cpp
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float32.hpp"

/**
 * @brief 智能车传感器数据订阅节点
 * 
 * 功能：订阅/sensor_data话题，接收传感器数据
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
        RCLCPP_INFO(this->get_logger(), 
                   "收到传感器数据：%.2f", 
                   msg->data);
        
        // 这里可以添加数据处理逻辑，比如：
        // - 判断是否超过阈值
        // - 触发相应控制逻辑
        // - 记录数据到日志
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

## 3. 服务（Service）示例

### 3.1 服务端：开关灯控制 (`service_server.cpp`)
```cpp
#include "rclcpp/rclcpp.hpp"
#include "std_srvs/srv/set_bool.hpp"

using std_srvs::srv::SetBool;

/**
 * @brief 智能车灯光控制服务端
 * 
 * 功能：提供/switch_light服务，控制智能车灯光开关
 * 对应智能车场景：控制GPIO连接的LED灯
 */
class LightServer : public rclcpp::Node {
public:
    LightServer() : Node("light_server") {
        // 创建服务，服务名为/switch_light
        service_ = this->create_service<SetBool>(
            "/switch_light",
            std::bind(&LightServer::handle_service, this,
                     std::placeholders::_1, std::placeholders::_2)
        );
        
        RCLCPP_INFO(this->get_logger(), "智能车灯光控制服务端已启动！等待请求...");
    }

private:
    void handle_service(
        const SetBool::Request::SharedPtr request,
        SetBool::Response::SharedPtr response) {
        
        // 根据请求数据执行相应操作
        if (request->data) {
            RCLCPP_INFO(this->get_logger(), "收到请求：开灯");
            
            // 模拟控制GPIO开灯
            // 实际项目中这里会调用硬件控制代码
            response->success = true;
            response->message = "智能车灯光已打开";
        } else {
            RCLCPP_INFO(this->get_logger(), "收到请求：关灯");
            
            // 模拟控制GPIO关灯
            response->success = true;
            response->message = "智能车灯光已关闭";
        }
        
        RCLCPP_INFO(this->get_logger(), "响应消息：%s", response->message.c_str());
    }
    
    rclcpp::Service<SetBool>::SharedPtr service_;
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<LightServer>());
    rclcpp::shutdown();
    return 0;
}
```

### 3.2 客户端：发送开关灯请求 (`service_client.cpp`)
```cpp
#include "rclcpp/rclcpp.hpp"
#include "std_srvs/srv/set_bool.hpp"

using std_srvs::srv::SetBool;
using namespace std::chrono_literals;

/**
 * @brief 智能车灯光控制客户端
 * 
 * 功能：向/switch_light服务发送开关灯请求
 * 对应智能车场景：远程控制智能车灯光
 */
class LightClient : public rclcpp::Node {
public:
    LightClient() : Node("light_client") {
        // 创建客户端
        client_ = this->create_client<SetBool>("/switch_light");
        
        // 等待服务端上线
        while (!client_->wait_for_service(1s)) {
            if (!rclcpp::ok()) {
                RCLCPP_ERROR(this->get_logger(), "客户端被中断");
                return;
            }
            RCLCPP_INFO(this->get_logger(), "等待服务端上线...");
        }
        
        // 发送开灯请求
        send_request(true);
    }

private:
    void send_request(bool turn_on) {
        auto request = std::make_shared<SetBool::Request>();
        request->data = turn_on;
        
        RCLCPP_INFO(this->get_logger(), "发送%s灯请求", turn_on ? "开" : "关");
        
        // 异步发送请求
        auto future = client_->async_send_request(request);
        
        // 等待响应
        if (rclcpp::spin_until_future_complete(this->shared_from_this(), future) ==
            rclcpp::FutureReturnCode::SUCCESS) {
            auto response = future.get();
            RCLCPP_INFO(this->get_logger(), 
                       "服务端响应：%s (成功：%s)", 
                       response->message.c_str(),
                       response->success ? "是" : "否");
        } else {
            RCLCPP_ERROR(this->get_logger(), "请求失败");
        }
    }
    
    rclcpp::Client<SetBool>::SharedPtr client_;
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    
    // 创建客户端节点
    auto node = std::make_shared<LightClient>();
    
    // 保持节点运行，可以发送更多请求
    rclcpp::spin(node);
    
    rclcpp::shutdown();
    return 0;
}
```

## 4. 动作（Action）示例

### 4.1 动作定义文件 (`action/Countdown.action`)
首先创建动作定义文件：
```
# 目标（Goal）：倒计时秒数
int32 seconds
---
# 结果（Result）：是否成功完成
bool success
string message
---
# 反馈（Feedback）：剩余秒数
int32 remaining_seconds
```

### 4.2 动作服务端：倒计时停车 (`action_server.cpp`)
```cpp
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "smart_car_core_demo/action/countdown.hpp"

#include <thread>
#include <memory>

using Countdown = smart_car_core_demo::action::Countdown;
using GoalHandle = rclcpp_action::ServerGoalHandle<Countdown>;

/**
 * @brief 智能车倒计时停车动作服务端
 * 
 * 功能：接收倒计时目标，每秒反馈剩余时间，完成后停车
 * 对应智能车场景：倒计时停车、定时任务执行
 */
class CountdownServer : public rclcpp::Node {
public:
    CountdownServer() : Node("countdown_server") {
        // 创建动作服务器
        action_server_ = rclcpp_action::create_server<Countdown>(
            this,
            "/countdown",
            std::bind(&CountdownServer::handle_goal, this,
                     std::placeholders::_1, std::placeholders::_2),
            std::bind(&CountdownServer::handle_cancel, this,
                     std::placeholders::_1),
            std::bind(&CountdownServer::handle_accepted, this,
                     std::placeholders::_1)
        );
        
        RCLCPP_INFO(this->get_logger(), "智能车倒计时停车动作服务端已启动！");
    }

private:
    rclcpp_action::Server<Countdown>::SharedPtr action_server_;
    
    // 处理目标请求
    rclcpp_action::GoalResponse handle_goal(
        const rclcpp_action::GoalUUID & uuid,
        std::shared_ptr<const Countdown::Goal> goal) {
        RCLCPP_INFO(this->get_logger(), 
                   "收到倒计时目标：%d秒", 
                   goal->seconds);
        (void)uuid;  // 避免未使用参数警告
        
        // 验证目标是否有效
        if (goal->seconds <= 0) {
            RCLCPP_WARN(this->get_logger(), "无效的倒计时时间：%d秒", goal->seconds);
            return rclcpp_action::GoalResponse::REJECT;
        }
        
        RCLCPP_INFO(this->get_logger(), "目标接受，开始执行");
        return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    }
    
    // 处理取消请求
    rclcpp_action::CancelResponse handle_cancel(
        const std::shared_ptr<GoalHandle> goal_handle) {
        RCLCPP_INFO(this->get_logger(), "收到取消请求");
        (void)goal_handle;
        
        // 接受取消请求
        return rclcpp_action::CancelResponse::ACCEPT;
    }
    
    // 执行目标
    void handle_accepted(const std::shared_ptr<GoalHandle> goal_handle) {
        // 在新线程中执行，避免阻塞
        std::thread{std::bind(&CountdownServer::execute, this, goal_handle)}.detach();
    }
    
    void execute(const std::shared_ptr<GoalHandle> goal_handle) {
        RCLCPP_INFO(this->get_logger(), "开始执行倒计时");
        
        auto feedback = std::make_shared<Countdown::Feedback>();
        auto result = std::make_shared<Countdown::Result>();
        
        auto goal = goal_handle->get_goal();
        int remaining_seconds = goal->seconds;
        
        // 倒计时循环
        for (int i = remaining_seconds; i >= 0; --i) {
            // 检查是否被取消
            if (goal_handle->is_canceling()) {
                result->success = false;
                result->message = "倒计时被取消";
                goal_handle->canceled(result);
                RCLCPP_INFO(this->get_logger(), "倒计时已取消");
                return;
            }
            
            // 发布反馈
            feedback->remaining_seconds = i;
            goal_handle->publish_feedback(feedback);
            
            RCLCPP_INFO(this->get_logger(), "倒计时剩余：%d秒", i);
            
            // 最后1秒时准备停车
            if (i == 1) {
                RCLCPP_INFO(this->get_logger(), "准备停车...");
            }
            
            // 等待1秒
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
        // 倒计时完成，停车
        result->success = true;
        result->message = "倒计时完成，智能车已停车";
        goal_handle->succeed(result);
        
        RCLCPP_INFO(this->get_logger(), "倒计时完成，智能车已安全停车");
    }
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<CountdownServer>());
    rclcpp::shutdown();
    return 0;
}
```

## 5. 整合节点示例

### 5.1 整合节点：车速发布+灯光控制 (`integrated_node.cpp`)
```cpp
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "std_srvs/srv/set_bool.hpp"

using namespace std::chrono_literals;
using std_srvs::srv::SetBool;

/**
 * @brief 智能车基础控制整合节点
 * 
 * 功能：整合话题发布和服务提供
 * 1. 发布车速指令到/cmd_vel话题
 * 2. 提供/switch_light服务控制灯光
 * 对应智能车场景：主控节点，同时处理运动控制和灯光控制
 */
class SmartCarControlNode : public rclcpp::Node {
public:
    SmartCarControlNode() : Node("smart_car_control") {
        // 1. 初始化话题发布者
        speed_publisher_ = this->create_publisher<geometry_msgs::msg::Twist>(
            "/cmd_vel", 10);
        
        // 2. 初始化服务
        light_service_ = this->create_service<SetBool>(
            "/switch_light",
            std::bind(&SmartCarControlNode::handle_light_service, this,
                     std::placeholders::_1, std::placeholders::_2));
        
        // 3. 创建定时器发布车速
        timer_ = this->create_wall_timer(
            100ms,
            std::bind(&SmartCarControlNode::publish_speed, this)
        );
        
        // 4. 初始化状态
        current_speed_ = 0.5;  // 默认速度
        current_turn_ = 0.2;   // 默认转向
        lights_on_ = false;    // 灯光状态
        
        RCLCPP_INFO(this->get_logger(), "智能车控制节点已启动！");
        RCLCPP_INFO(this->get_logger(), "功能：1. 发布车速指令 2. 控制灯光");
    }

private:
    // 发布车速指令
    void publish_speed() {
        auto msg = geometry_msgs::msg::Twist();
        msg.linear.x = current_speed_;
        msg.angular.z = current_turn_;
        
        speed_publisher_->publish(msg);
        
        // 每10次打印一次日志
        static int
