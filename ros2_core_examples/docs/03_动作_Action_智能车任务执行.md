# ROS2动作（Action）示例 - 智能车任务执行

## 概述
动作是ROS2中的高级通信机制，适用于长时间运行的任务，支持：
- 目标（Goal）：启动任务
- 反馈（Feedback）：任务执行过程中的进度更新
- 结果（Result）：任务完成后的最终结果

在智能车中，动作适用于：
- 导航到指定位置
- 执行复杂轨迹
- 数据采集任务
- 自动充电等长时间操作

## 1. 智能车倒计时停车动作

### 功能描述
- 动作名称：`/countdown_park`
- 目标：倒计时指定秒数后停车
- 反馈：每秒发送剩余时间
- 结果：停车是否成功
- 支持取消操作

### 动作定义
创建 `action/CountdownPark.action` 文件：
```
# 目标：倒计时秒数
int32 seconds
---
# 结果：停车结果
bool success
string message
float32 final_speed
---
# 反馈：剩余秒数
int32 remaining_seconds
float32 current_speed
```

### 动作服务端实现 (`countdown_park_server.cpp`)

```cpp
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "smart_car_core_demo/action/countdown_park.hpp"

#include <thread>
#include <memory>
#include <chrono>

using CountdownPark = smart_car_core_demo::action::CountdownPark;
using GoalHandle = rclcpp_action::ServerGoalHandle<CountdownPark>;

/**
 * @brief 智能车倒计时停车动作服务端
 * 
 * 功能：接收倒计时目标，每秒反馈进度，完成后停车
 * 对应智能车场景：安全停车、定时任务
 */
class CountdownParkServer : public rclcpp::Node {
public:
    CountdownParkServer() : Node("countdown_park_server") {
        // 创建动作服务器
        action_server_ = rclcpp_action::create_server<CountdownPark>(
            this,
            "/countdown_park",
            std::bind(&CountdownParkServer::handle_goal, this,
                     std::placeholders::_1, std::placeholders::_2),
            std::bind(&CountdownParkServer::handle_cancel, this,
                     std::placeholders::_1),
            std::bind(&CountdownParkServer::handle_accepted, this,
                     std::placeholders::_1)
        );
        
        // 初始化当前速度
        current_speed_ = 0.5f;
        
        RCLCPP_INFO(this->get_logger(), "智能车倒计时停车动作服务端已启动！");
        RCLCPP_INFO(this->get_logger(), "当前速度：%.2f m/s", current_speed_);
    }

private:
    rclcpp_action::Server<CountdownPark>::SharedPtr action_server_;
    float current_speed_;
    
    // 处理目标请求
    rclcpp_action::GoalResponse handle_goal(
        const rclcpp_action::GoalUUID & uuid,
        std::shared_ptr<const CountdownPark::Goal> goal) {
        
        RCLCPP_INFO(this->get_logger(), 
                   "收到倒计时停车目标：%d秒", 
                   goal->seconds);
        (void)uuid;  // 避免未使用参数警告
        
        // 验证目标是否有效
        if (goal->seconds <= 0) {
            RCLCPP_WARN(this->get_logger(), 
                       "无效的倒计时时间：%d秒，必须大于0", 
                       goal->seconds);
            return rclcpp_action::GoalResponse::REJECT;
        }
        
        if (goal->seconds > 60) {
            RCLCPP_WARN(this->get_logger(), 
                       "倒计时时间过长：%d秒，建议不超过60秒", 
                       goal->seconds);
        }
        
        RCLCPP_INFO(this->get_logger(), "目标接受，开始执行倒计时停车");
        return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    }
    
    // 处理取消请求
    rclcpp_action::CancelResponse handle_cancel(
        const std::shared_ptr<GoalHandle> goal_handle) {
        RCLCPP_INFO(this->get_logger(), "收到取消请求");
        
        // 检查目标是否仍在执行
        if (goal_handle->is_executing()) {
            RCLCPP_WARN(this->get_logger(), "正在取消倒计时停车任务");
            return rclcpp_action::CancelResponse::ACCEPT;
        }
        
        RCLCPP_INFO(this->get_logger(), "任务已完成或未开始，取消无效");
        return rclcpp_action::CancelResponse::REJECT;
    }
    
    // 执行目标
    void handle_accepted(const std::shared_ptr<GoalHandle> goal_handle) {
        // 在新线程中执行，避免阻塞
        std::thread{std::bind(&CountdownParkServer::execute, this, goal_handle)}.detach();
    }
    
    void execute(const std::shared_ptr<GoalHandle> goal_handle) {
        RCLCPP_INFO(this->get_logger(), "开始执行倒计时停车");
        
        auto feedback = std::make_shared<CountdownPark::Feedback>();
        auto result = std::make_shared<CountdownPark::Result>();
        
        auto goal = goal_handle->get_goal();
        int remaining_seconds = goal->seconds;
        
        // 倒计时循环
        for (int i = remaining_seconds; i >= 0; --i) {
            // 检查是否被取消
            if (goal_handle->is_canceling()) {
                result->success = false;
                result->message = "倒计时停车被取消";
                result->final_speed = current_speed_;
                goal_handle->canceled(result);
                
                RCLCPP_WARN(this->get_logger(), 
                           "倒计时停车已取消，当前速度：%.2f m/s", 
                           current_speed_);
                return;
            }
            
            // 更新反馈
            feedback->remaining_seconds = i;
            feedback->current_speed = current_speed_;
            goal_handle->publish_feedback(feedback);
            
            // 根据剩余时间调整速度
            if (i <= 3) {
                // 最后3秒开始减速
                current_speed_ = 0.5f * (i / 3.0f);
                RCLCPP_INFO(this->get_logger(), 
                           "减速中... 剩余%d秒，速度：%.2f m/s", 
                           i, current_speed_);
            } else {
                RCLCPP_INFO(this->get_logger(), 
                           "倒计时剩余：%d秒，速度：%.2f m/s", 
                           i, current_speed_);
            }
            
            // 等待1秒
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
        // 倒计时完成，完全停车
        current_speed_ = 0.0f;
        result->success = true;
        result->message = "倒计时完成，智能车已安全停车";
        result->final_speed = current_speed_;
        goal_handle->succeed(result);
        
        RCLCPP_INFO(this->get_logger(), "倒计时完成，智能车已安全停车");
        RCLCPP_INFO(this->get_logger(), "最终速度：%.2f m/s", current_speed_);
    }
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<CountdownParkServer>());
    rclcpp::shutdown();
    return 0;
}
```

## 2. 智能车导航到指定位置动作

### 功能描述
- 动作名称：`/navigate_to_position`
- 目标：导航到指定坐标 (x, y)
- 反馈：当前位置和剩余距离
- 结果：是否成功到达目标

### 动作定义
创建 `action/NavigateToPosition.action` 文件：
```
# 目标：目标位置
float32 target_x
float32 target_y
---
# 结果：导航结果
bool success
string message
float32 final_distance
---
# 反馈：导航进度
float32 current_x
float32 current_y
float32 remaining_distance
float32 progress_percentage
```

## 3. 智能车数据采集动作

### 功能描述
- 动作名称：`/collect_sensor_data`
- 目标：采集指定时长的传感器数据
- 反馈：已采集数据量和时间
- 结果：采集的数据文件路径

### 动作定义
创建 `action/CollectSensorData.action` 文件：
```
# 目标：采集参数
int32 duration_seconds  # 采集时长
string sensor_type      # 传感器类型
---
# 结果：采集结果
bool success
string message
string data_file_path   # 数据文件路径
int32 data_points       # 数据点数
---
# 反馈：采集进度
int32 elapsed_seconds
int32 collected_points
float32 progress_percentage
```

## 4. 动作客户端实现

### 通用动作客户端 (`action_client.cpp`)

```cpp
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "smart_car_core_demo/action/countdown_park.hpp"

#include <future>

using CountdownPark = smart_car_core_demo::action::CountdownPark;
using GoalHandle = rclcpp_action::ClientGoalHandle<CountdownPark>;

/**
 * @brief 智能车动作客户端
 * 
 * 功能：向动作服务器发送目标，接收反馈和结果
 * 对应智能车场景：远程任务控制
 */
class ActionClient : public rclcpp::Node {
public:
    ActionClient() : Node("action_client") {
        // 创建动作客户端
        client_ = rclcpp_action::create_client<CountdownPark>(
            this,
            "/countdown_park"
        );
        
        RCLCPP_INFO(this->get_logger(), "智能车动作客户端已启动");
        
        // 发送倒计时停车目标
        send_goal(5);  // 5秒倒计时
    }

private:
    rclcpp_action::Client<CountdownPark>::SharedPtr client_;
    
    void send_goal(int seconds) {
        // 等待动作服务器
        if (!client_->wait_for_action_server(std::chrono::seconds(5))) {
            RCLCPP_ERROR(this->get_logger(), "动作服务器未响应");
            return;
        }
        
        // 创建目标
        auto goal = CountdownPark::Goal();
        goal.seconds = seconds;
        
        RCLCPP_INFO(this->get_logger(), 
                   "发送倒计时停车目标：%d秒", 
                   seconds);
        
        // 设置目标选项
        auto send_goal_options = rclcpp_action::Client<CountdownPark>::SendGoalOptions();
        send_goal_options.goal_response_callback =
            std::bind(&ActionClient::goal_response_callback, this, std::placeholders::_1);
        send_goal_options.feedback_callback =
            std::bind(&ActionClient::feedback_callback, this, 
                     std::placeholders::_1, std::placeholders::_2);
        send_goal_options.result_callback =
            std::bind(&ActionClient::result_callback, this, std::placeholders::_1);
        
        // 发送目标
        client_->async_send_goal(goal, send_goal_options);
    }
    
    // 目标响应回调
    void goal_response_callback(std::shared_future<GoalHandle::SharedPtr> future) {
        auto goal_handle = future.get();
        if (!goal_handle) {
            RCLCPP_ERROR(this->get_logger(), "目标被拒绝");
        } else {
            RCLCPP_INFO(this->get_logger(), "目标已接受，正在执行");
        }
    }
    
    // 反馈回调
    void feedback_callback(
        GoalHandle::SharedPtr,
        const std::shared_ptr<const CountdownPark::Feedback> feedback) {
        
        RCLCPP_INFO(this->get_logger(),
                   "收到反馈：剩余%d秒，当前速度：%.2f m/s",
                   feedback->remaining_seconds,
                   feedback->current_speed);
        
        // 根据反馈更新UI或执行其他操作
        if (feedback->remaining_seconds <= 3) {
            RCLCPP_WARN(this->get_logger(), "即将停车，请做好准备");
        }
    }
    
    // 结果回调
    void result_callback(const GoalHandle::WrappedResult & result) {
        switch (result.code) {
            case rclcpp_action::ResultCode::SUCCEEDED:
                RCLCPP_INFO(this->get_logger(),
                           "动作成功完成：%s，最终速度：%.2f m/s",
                           result.result->message.c_str(),
                           result.result->final_speed);
                break;
            case rclcpp_action::ResultCode::ABORTED:
                RCLCPP_ERROR(this->get_logger(),
                           "动作被中止：%s",
                           result.result->message.c_str());
                break;
            case rclcpp_action::ResultCode::CANCELED:
                RCLCPP_WARN(this->get_logger(),
                           "动作被取消：%s",
                           result.result->message.c_str());
                break;
            default:
                RCLCPP_ERROR(this->get_logger(), "未知结果代码");
                break;
        }
    }
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    
    // 创建客户端节点
    auto node = std::make_shared<ActionClient>();
    
    // 保持节点运行
    rclcpp::spin(node);
    
    rclcpp::shutdown();
    return 0;
}
```

## 5. 编译配置

### CMakeLists.txt 配置
```cmake
# 生成动作接口
find_package(rosidl_default_generators REQUIRED)
rosidl_generate_interfaces(${PROJECT_NAME}
  "action/CountdownPark.action"
  "action/NavigateToPosition.action"
  "action/CollectSensorData.action"
)

# 添加动作服务端可执行文件
add_executable(countdown_park_server src/countdown_park_server.cpp)
ament_target_dependencies(countdown_park_server rclcpp rclcpp_action)
target_link_libraries(countdown_park_server 
    ${PROJECT_NAME}__rosidl_generator_c 
    ${PROJECT_NAME}__rosidl_typesupport_c
    ${PROJECT_NAME}__rosidl_typesupport_cpp
)

# 添加动作客户端可执行文件
add_executable(action_client src/action_client.cpp)
ament_target_dependencies(action_client rclcpp rclcpp_action)
target_link_libraries(action_client 
    ${PROJECT_NAME}__rosidl_generator_c 
    ${PROJECT_NAME}__rosidl_typesupport_c
    ${PROJECT_NAME}__rosidl_typesupport_cpp
)

# 安装可执行文件
install(TARGETS countdown_park_server action_client
  DESTINATION lib/${PROJECT_NAME}
)
```

### package.xml 依赖
```xml
<depend>rclcpp</depend>
<depend>rclcpp_action</depend>
<build_depend>rosidl_default_generators</build_depend>
<exec_depend>rosidl_default_runtime</exec_depend>
<member_of_group>rosidl_interface_packages</member_of_group>
```

## 6. 运行测试

### 测试步骤
1. **编译包**：
   ```bash
   cd ~/ros2_ws
   colcon build --packages-select smart_car_core_demo
   source install/setup.bash
   ```

2. **运行倒计时停车服务端**：
   ```bash
   ros2 run smart_car_core_demo countdown_park_server
   ```

3. **运行动作客户端**（新终端）：
   ```bash
   ros2 run smart_car_core_demo action_client
   ```
   输出：显示倒计时进度和最终结果

4. **使用命令行测试动作**：
   ```bash
   # 发送倒计时停车目标
   ros2 action send_goal /countdown_park smart_car_core_demo/action/CountdownPark "{seconds: 5}" --feedback
   
   # 取消正在执行的动作（需要知道goal_id）
   ros2 action cancel_goal /countdown_park <goal_id>
   ```

5. **监控动作状态**：
   ```bash
   # 查看动作列表
   ros2 action list
   
   # 查看动作信息
   ros2 action info /countdown_park
   ```

## 7. 实际智能车应用建议

### 动作设计原则
1. **长时间任务**：动作适用于需要较长时间完成的任务
2. **进度反馈**：定期发送反馈，让客户端了解进度
3. **可取消性**：支持任务取消，提高系统灵活性
4. **结果明确**：任务完成后返回明确的结果

### 智能车动作场景
1. **自动导航**：
   ```cpp
   // 目标：导航到(x=2.0, y=3.0)
   goal.target_x = 2.0f;
   goal.target_y = 3.0f;
   ```

2. **路径跟踪**：
   ```cpp
   // 目标：跟踪指定路径点
   goal.waypoints = {{1,1}, {2,2}, {3,1}};
   ```

3. **自动充电**：
   ```cpp
   // 目标：返回充电站并充电
   goal.charge_to_percentage = 100;
   ```

### 性能优化
1. **反馈频率**：根据任务类型调整反馈频率（通常1-10Hz）
2. **线程管理**：使用独立线程执行长时间任务，避免阻塞
3. **资源清理**：任务完成后及时清理资源

## 8. 错误处理与恢复

### 常见错误处理
```cpp
try {
    // 执行动作任务
    execute_task(goal_handle);
} catch (const std::exception &e) {
    // 任务执行
