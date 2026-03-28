# ROS2服务（Service）示例 - 智能车控制命令

## 概述
服务是ROS2中的请求-响应通信机制，适用于需要确认结果的命令。在智能车中，服务用于：
- 发送控制命令并获取执行结果
- 查询车辆状态
- 执行一次性配置操作
- 启动/停止特定功能

## 1. 智能车紧急停止服务

### 功能描述
- 提供 `/emergency_stop` 服务
- 客户端发送停止请求
- 服务端执行紧急停止并返回结果
- 适用于安全关键场景

### 服务定义
创建 `srv/EmergencyStop.srv` 文件：
```
# 请求：是否执行紧急停止
bool stop_request
---
# 响应：停止结果
bool success
string message
float32 current_speed  # 停止前的速度
```

### 服务端实现 (`emergency_stop_server.cpp`)

```cpp
#include "rclcpp/rclcpp.hpp"
#include "smart_car_core_demo/srv/emergency_stop.hpp"

using EmergencyStop = smart_car_core_demo::srv::EmergencyStop;

/**
 * @brief 智能车紧急停止服务端
 * 
 * 功能：提供紧急停止服务，确保智能车安全停止
 * 对应智能车场景：安全控制，防止碰撞
 */
class EmergencyStopServer : public rclcpp::Node {
public:
    EmergencyStopServer() : Node("emergency_stop_server") {
        // 模拟当前车速
        current_speed_ = 0.5f;
        
        // 创建紧急停止服务
        service_ = this->create_service<EmergencyStop>(
            "/emergency_stop",
            std::bind(&EmergencyStopServer::handle_service, this,
                     std::placeholders::_1, std::placeholders::_2)
        );
        
        RCLCPP_INFO(this->get_logger(), "智能车紧急停止服务端已启动！");
        RCLCPP_INFO(this->get_logger(), "当前模拟车速：%.2f m/s", current_speed_);
    }

private:
    void handle_service(
        const EmergencyStop::Request::SharedPtr request,
        EmergencyStop::Response::SharedPtr response) {
        
        RCLCPP_INFO(this->get_logger(), "收到紧急停止请求");
        
        if (request->stop_request) {
            // 执行紧急停止逻辑
            float stopped_speed = current_speed_;
            current_speed_ = 0.0f;
            
            response->success = true;
            response->message = "智能车已紧急停止";
            response->current_speed = stopped_speed;
            
            RCLCPP_WARN(this->get_logger(), 
                       "执行紧急停止！停止前速度：%.2f m/s", 
                       stopped_speed);
            
            // 在实际智能车中，这里会：
            // 1. 发送零速度指令到电机
            // 2. 激活刹车系统
            // 3. 记录停止事件到日志
            // 4. 通知其他节点
        } else {
            response->success = false;
            response->message = "无效的停止请求";
            response->current_speed = current_speed_;
            
            RCLCPP_WARN(this->get_logger(), "收到无效的停止请求");
        }
    }
    
    rclcpp::Service<EmergencyStop>::SharedPtr service_;
    float current_speed_;  // 模拟当前车速
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<EmergencyStopServer>());
    rclcpp::shutdown();
    return 0;
}
```

### 客户端实现 (`emergency_stop_client.cpp`)

```cpp
#include "rclcpp/rclcpp.hpp"
#include "smart_car_core_demo/srv/emergency_stop.hpp"

using EmergencyStop = smart_car_core_demo::srv::EmergencyStop;
using namespace std::chrono_literals;

/**
 * @brief 智能车紧急停止客户端
 * 
 * 功能：向紧急停止服务发送请求
 * 对应智能车场景：远程安全控制
 */
class EmergencyStopClient : public rclcpp::Node {
public:
    EmergencyStopClient() : Node("emergency_stop_client") {
        // 创建客户端
        client_ = this->create_client<EmergencyStop>("/emergency_stop");
        
        // 等待服务端上线
        wait_for_service();
        
        // 发送紧急停止请求
        send_emergency_stop();
    }

private:
    void wait_for_service() {
        while (!client_->wait_for_service(1s)) {
            if (!rclcpp::ok()) {
                RCLCPP_ERROR(this->get_logger(), "客户端被中断");
                return;
            }
            RCLCPP_INFO(this->get_logger(), "等待紧急停止服务端上线...");
        }
        RCLCPP_INFO(this->get_logger(), "服务端已连接");
    }
    
    void send_emergency_stop() {
        auto request = std::make_shared<EmergencyStop::Request>();
        request->stop_request = true;
        
        RCLCPP_WARN(this->get_logger(), "发送紧急停止请求...");
        
        // 异步发送请求
        auto future = client_->async_send_request(request);
        
        // 等待响应（带超时）
        auto status = future.wait_for(5s);
        
        if (status == std::future_status::ready) {
            try {
                auto response = future.get();
                if (response->success) {
                    RCLCPP_ERROR(this->get_logger(), 
                               "紧急停止成功！消息：%s，停止前速度：%.2f m/s",
                               response->message.c_str(),
                               response->current_speed);
                } else {
                    RCLCPP_WARN(this->get_logger(), 
                               "紧急停止失败：%s",
                               response->message.c_str());
                }
            } catch (const std::exception &e) {
                RCLCPP_ERROR(this->get_logger(), "服务调用异常：%s", e.what());
            }
        } else {
            RCLCPP_ERROR(this->get_logger(), "服务调用超时");
        }
    }
    
    rclcpp::Client<EmergencyStop>::SharedPtr client_;
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    
    // 创建客户端节点
    auto node = std::make_shared<EmergencyStopClient>();
    
    // 保持节点运行
    rclcpp::spin(node);
    
    rclcpp::shutdown();
    return 0;
}
```

## 2. 智能车状态查询服务

### 功能描述
- 提供 `/query_status` 服务
- 客户端查询智能车当前状态
- 返回电池、传感器、电机等状态信息

### 服务定义
创建 `srv/QueryStatus.srv` 文件：
```
# 请求：查询类型
string query_type  # "all", "battery", "sensors", "motors"
---
# 响应：状态信息
bool success
string message
string status_json  # JSON格式的状态信息
```

### 服务端实现 (`status_server.cpp`)

```cpp
#include "rclcpp/rclcpp.hpp"
#include "smart_car_core_demo/srv/query_status.hpp"
#include <nlohmann/json.hpp>

using QueryStatus = smart_car_core_demo::srv::QueryStatus;
using json = nlohmann::json;

/**
 * @brief 智能车状态查询服务端
 * 
 * 功能：提供智能车状态查询服务
 * 对应智能车场景：状态监控、调试
 */
class StatusServer : public rclcpp::Node {
public:
    StatusServer() : Node("status_server") {
        // 创建状态查询服务
        service_ = this->create_service<QueryStatus>(
            "/query_status",
            std::bind(&StatusServer::handle_service, this,
                     std::placeholders::_1, std::placeholders::_2)
        );
        
        // 初始化模拟状态
        init_status();
        
        RCLCPP_INFO(this->get_logger(), "智能车状态查询服务端已启动！");
    }

private:
    void init_status() {
        // 模拟智能车状态
        status_["battery"] = {
            {"voltage", 12.8},
            {"percentage", 85},
            {"charging", false}
        };
        
        status_["sensors"] = {
            {"lidar", "connected"},
            {"imu", "connected"},
            {"camera", "disconnected"}
        };
        
        status_["motors"] = {
            {"left_speed", 0.5},
            {"right_speed", 0.5},
            {"temperature", 45.2}
        };
        
        status_["system"] = {
            {"uptime", 3600},  // 秒
            {"cpu_usage", 32.5},  // 百分比
            {"memory_usage", 45.8}  // 百分比
        };
    }
    
    void handle_service(
        const QueryStatus::Request::SharedPtr request,
        QueryStatus::Response::SharedPtr response) {
        
        RCLCPP_INFO(this->get_logger(), "收到状态查询请求：%s", 
                   request->query_type.c_str());
        
        try {
            json result;
            
            if (request->query_type == "all") {
                result = status_;
            } else if (request->query_type == "battery") {
                result = status_["battery"];
            } else if (request->query_type == "sensors") {
                result = status_["sensors"];
            } else if (request->query_type == "motors") {
                result = status_["motors"];
            } else if (request->query_type == "system") {
                result = status_["system"];
            } else {
                throw std::runtime_error("未知的查询类型: " + request->query_type);
            }
            
            response->success = true;
            response->message = "查询成功";
            response->status_json = result.dump(2);  // 缩进2空格
            
            RCLCPP_INFO(this->get_logger(), "返回状态信息");
            
        } catch (const std::exception &e) {
            response->success = false;
            response->message = std::string("查询失败: ") + e.what();
            response->status_json = "{}";
            
            RCLCPP_ERROR(this->get_logger(), "状态查询失败：%s", e.what());
        }
    }
    
    rclcpp::Service<QueryStatus>::SharedPtr service_;
    json status_;  // 智能车状态
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<StatusServer>());
    rclcpp::shutdown();
    return 0;
}
```

## 3. 智能车配置服务

### 功能描述
- 提供 `/configure_car` 服务
- 客户端发送配置参数
- 服务端应用配置并返回结果

### 服务定义
创建 `srv/ConfigureCar.srv` 文件：
```
# 请求：配置参数
string config_type  # "speed_limit", "safety_mode", "control_mode"
float32 value
---
# 响应：配置结果
bool success
string message
string new_config
```

## 4. 编译配置

### CMakeLists.txt 配置
```cmake
# 生成服务接口
find_package(rosidl_default_generators REQUIRED)
rosidl_generate_interfaces(${PROJECT_NAME}
  "srv/EmergencyStop.srv"
  "srv/QueryStatus.srv"
  "srv/ConfigureCar.srv"
)

# 添加服务端可执行文件
add_executable(emergency_stop_server src/emergency_stop_server.cpp)
ament_target_dependencies(emergency_stop_server rclcpp)
target_link_libraries(emergency_stop_server ${PROJECT_NAME}__rosidl_generator_c ${PROJECT_NAME}__rosidl_typesupport_c)

add_executable(status_server src/status_server.cpp)
ament_target_dependencies(status_server rclcpp nlohmann_json::nlohmann_json)
target_link_libraries(status_server ${PROJECT_NAME}__rosidl_generator_c ${PROJECT_NAME}__rosidl_typesupport_c)

# 添加客户端可执行文件
add_executable(emergency_stop_client src/emergency_stop_client.cpp)
ament_target_dependencies(emergency_stop_client rclcpp)
target_link_libraries(emergency_stop_client ${PROJECT_NAME}__rosidl_generator_c ${PROJECT_NAME}__rosidl_typesupport_c)

# 安装可执行文件
install(TARGETS emergency_stop_server status_server emergency_stop_client
  DESTINATION lib/${PROJECT_NAME}
)
```

### package.xml 依赖
```xml
<depend>rclcpp</depend>
<build_depend>rosidl_default_generators</build_depend>
<exec_depend>rosidl_default_runtime</exec_depend>
<member_of_group>rosidl_interface_packages</member_of_group>
<depend>nlohmann_json</depend>
```

## 5. 运行测试

### 测试步骤
1. **编译包**：
   ```bash
   cd ~/ros2_ws
   colcon build --packages-select smart_car_core_demo
   source install/setup.bash
   ```

2. **运行紧急停止服务端**：
   ```bash
   ros2 run smart_car_core_demo emergency_stop_server
   ```

3. **运行紧急停止客户端**（新终端）：
   ```bash
   ros2 run smart_car_core_demo emergency_stop_client
   ```
   输出：显示紧急停止结果

4. **运行状态查询服务端**（新终端）：
   ```bash
   ros2 run smart_car_core_demo status_server
   ```

5. **使用命令行测试服务**：
   ```bash
   # 查询所有状态
   ros2 service call /query_status smart_car_core_demo/srv/QueryStatus "{query_type: 'all'}"
   
   # 查询电池状态
   ros2 service call /query_status smart_car_core_demo/srv/QueryStatus "{query_type: 'battery'}"
   
   # 发送紧急停止
   ros2 service call /emergency_stop smart_car_core_demo/srv/EmergencyStop "{stop_request: true}"
   ```

## 6. 实际智能车应用建议

### 服务设计原则
1. **原子性**：每个服务完成一个明确的任务
2. **幂等性**：多次调用相同请求应产生相同结果
3. **超时处理**：客户端应设置合理的超时时间
4. **错误处理**：服务端应返回详细的错误信息

### 服务类型选择
- **同步服务**：适用于需要立即响应的命令（如紧急停止）
- **异步服务**：适用于耗时操作（如数据采集、复杂计算）

### 性能考虑
1. **响应时间**：关键服务应在100ms内响应
2. **并发处理**：服务端应能处理多个并发请求
3. **资源占用**：避免在服务回调中进行耗时操作

## 7. 智能车服务场景示例

### 场景1：远程控制命令
```cpp
// 客户端发送前进命令
auto request = std::make_shared<ControlCommand::Request>();
request->command = "move_forward";
request->distance = 2.0;  // 前进2米
```

### 场景2：系统诊断
```cpp
// 客户端请求系统诊断
auto request = std::make_shared<Diagnostic::Request>();
request->check_level = "full";  // 完整诊断
```

### 场景3：参数配置
```cpp
// 客户端配置最大速度
auto request = std::make_shared<Configure::Request>();
request->param_name = "max_speed";
request->param_value = "1.5";  // 1.5 m/s
```

## 总结
服务是智能车ROS2系统中重要的命令-响应机制。通过本示例，您可以：
1. 掌握服务的基本使用模式
2. 了解智能车常用服务类型
3. 学会设计实用的智能车服务接口
4. 为后续的动作学习打下基础

**实际应用建议**：
1. 将紧急停止服务集成到您的智能车安全系统中
2. 使用状态查询服务进行调试和监控
3. 根据实际需求设计更多定制化服务

**下一步**：尝试创建您自己的智能车服务，如"设置导航目标"、"切换控制模式"等。
