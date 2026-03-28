// 标准C++算法库：提供排序、查找、最大最小值等通用算法
#include <algorithm>
// 标准数学库：提供三角函数、开方、绝对值、取整等数学运算
#include <cmath>
// 系统信号处理库：处理Ctrl+C关闭、程序异常退出等系统信号
#include <csignal>
// 函数对象/回调库：用于函数传递、绑定，ROS2中大量用于订阅者回调
#include <functional>
// 标准输入输出库：用于控制台打印日志、输出调试信息
#include <iostream>
// 内存管理库：提供智能指针（安全管理内存），ROS2创建节点必备
#include <memory>
// 多线程库：用于创建并行线程，实现程序同时执行多个任务
#include <thread>
// 动态数组容器：最常用的可变长数组，用于存储传感器数据、列表等
#include <vector>

#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"
#include "ros_robot_controller_msgs/msg/motor_state.hpp"
#include "ros_robot_controller_msgs/msg/motors_state.hpp"

using namespace std::chrono_literals;  // C++14 引入的时间字面量操作符
// 例如：
// - `1s` 表示 1 秒
// - `500ms` 表示 500 毫秒
// - `2min` 表示 2 分钟
// - `3h` 表示 3 小时

/**
 * @brief 麦克纳姆轮底盘运动学类 (Mecanum wheel chassis kinematics)
 *
 * 将线速度和角速度转换为四个电机的转速
 */
class MecanumChassis {
  public:
    /**
     * @brief 构造函数
     * @param wheelbase 前后轴距 (m)
     * @param track_width 左右轴距 (m)
     * @param wheel_diameter 轮子直径 (m)
     */
    MecanumChassis(double wheelbase = 0.216, double track_width = 0.195, double wheel_diameter = 0.097)
        : wheelbase_(wheelbase), track_width_(track_width), wheel_diameter_(wheel_diameter) {}

    /**
     * @brief 速度转换：m/s 转 rps (转/秒)
     * @param speed 线速度 (m/s)
     * @return 转速 (rps)
     */
    double speedToRps(double speed) const { return speed / (M_PI * wheel_diameter_); }

    /**
     * @brief 设置速度
     * @param linear_x X方向线速度 (m/s)
     * @param linear_y Y方向线速度 (m/s)
     * @param angular_z Z方向角速度 (rad/s)
     * @return MotorsState消息，包含四个电机的转速
     */
    ros_robot_controller_msgs::msg::MotorsState setVelocity(double linear_x, double linear_y, double angular_z) const {
        // 麦轮运动学公式
        double factor = (wheelbase_ + track_width_) / 2.0;
        double motor1 = linear_x - linear_y - angular_z * factor;
        double motor2 = linear_x + linear_y - angular_z * factor;
        double motor3 = linear_x + linear_y + angular_z * factor;
        double motor4 = linear_x - linear_y + angular_z * factor;

        // 转换为rps
        std::vector<double> speeds = {speedToRps(motor1),
                                      speedToRps(motor2),
                                      speedToRps(-motor3),  // 注意：电机3和4方向相反
                                      speedToRps(-motor4)};

        // 创建MotorsState消息
        ros_robot_controller_msgs::msg::MotorsState motors_msg;

        for (size_t i = 0; i < speeds.size(); ++i) {
            // 完整定义
            ros_robot_controller_msgs::msg::MotorState motor_state;
            // auto motor_state = ros_robot_controller_msgs::msg::MotorState();
            motor_state.id  = static_cast<uint16_t>(i + 1);  // 电机ID从1开始
            motor_state.rps = speeds[i];
            motors_msg.data.push_back(motor_state);
        }

        return motors_msg;
    }

  private:
    double wheelbase_;
    double track_width_;
    double wheel_diameter_;
};

/**
 * @brief 智能车速度订阅节点
 *
 * 功能：订阅/cmd_vel话题，计算电机转速并发布到/ros_robot_controller/set_motor话题
 * 对应智能车场景：控制麦轮小车运动
 */
class SpeedSubscriber : public rclcpp::Node {
  public:
    SpeedSubscriber() : Node("speed_subscriber"), chassis_(0.216, 0.195, 0.097) {
        // 创建订阅者，订阅cmd_vel话题
        cmd_vel_subscription_ = this->create_subscription<geometry_msgs::msg::Twist>(
            "cmd_vel",
            10,
            [this](const geometry_msgs::msg::Twist::SharedPtr msg) { this->cmdVelCallback(msg); });

        // 创建发布者，发布电机速度到ros_robot_controller/set_motor话题
        motor_publisher_ =
            this->create_publisher<ros_robot_controller_msgs::msg::MotorsState>("ros_robot_controller/set_motor", 10);

        RCLCPP_INFO(this->get_logger(), "速度订阅节点已启动，等待cmd_vel指令...");
    }

    /**
     * @brief 紧急停止所有电机
     * 
     * 在接收到关闭信号时调用此方法停止所有电机
     */
    void emergencyStop() {
        RCLCPP_INFO(this->get_logger(), "执行紧急停止，停止所有电机...");
        
        // 创建停止消息
        auto stop_msg = ros_robot_controller_msgs::msg::MotorsState();
        for (int i = 1; i <= 4; ++i) {
            auto motor_state = ros_robot_controller_msgs::msg::MotorState();
            motor_state.id = i;
            motor_state.rps = 0.0;
            stop_msg.data.push_back(motor_state);
        }
        
        // 使用现有发布者发布停止指令
        motor_publisher_->publish(stop_msg);
        
        RCLCPP_INFO(this->get_logger(), "已发布停止指令到所有4个电机");
    }

    /**
     * @brief 获取电机发布者（用于信号处理）
     * @return 电机发布者的共享指针
     */
    rclcpp::Publisher<ros_robot_controller_msgs::msg::MotorsState>::SharedPtr getMotorPublisher() {
        return motor_publisher_;
    }

  private:
    // 运动学计算器
    MecanumChassis chassis_;
    /**
     * @brief cmd_vel话题回调函数
     * @param msg 速度指令消息
     */
    void cmdVelCallback(const geometry_msgs::msg::Twist::SharedPtr msg) {
        // 限制速度范围，确保安全
        double linear_x  = msg->linear.x;
        double linear_y  = msg->linear.y;
        double angular_z = msg->angular.z;

        // 手动实现clamp功能，避免C++17依赖
        if (linear_x > 0.5) linear_x = 0.5;
        if (linear_x < -0.5) linear_x = -0.5;
        if (linear_y > 0.2) linear_y = 0.2;
        if (linear_y < -0.2) linear_y = -0.2;
        if (angular_z > 0.5) angular_z = 0.5;
        if (angular_z < -0.5) angular_z = -0.5;

        // 使用运动学计算电机速度
        auto motors_msg = chassis_.setVelocity(linear_x, linear_y, angular_z);

        // 发布电机速度指令
        motor_publisher_->publish(motors_msg);

        // 打印日志
        RCLCPP_INFO(this->get_logger(),
                    "收到速度指令: 线速度(%.2f, %.2f) m/s, 角速度%.2f rad/s",
                    linear_x,
                    linear_y,
                    angular_z);

        RCLCPP_INFO(this->get_logger(),
                    "发布电机转速: 1:%.2f, 2:%.2f, 3:%.2f, 4:%.2f rps",
                    motors_msg.data[0].rps,
                    motors_msg.data[1].rps,
                    motors_msg.data[2].rps,
                    motors_msg.data[3].rps);
    }

    // 使用智能指针管理订阅者和发布者
    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr                cmd_vel_subscription_;
    rclcpp::Publisher<ros_robot_controller_msgs::msg::MotorsState>::SharedPtr motor_publisher_;
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);

    // 使用智能指针创建节点
    auto node = std::make_shared<SpeedSubscriber>();

    // 设置信号处理：当接收到SIGINT（Ctrl+C）时停止电机
    // 注意：在ROS2中，rclcpp::spin()会自动处理信号，但我们需要确保电机停止
    // 这里我们使用标准的signal函数来捕获信号
    std::signal(SIGINT, [](int sig) {
        // 这个处理函数在全局上下文中运行，无法直接访问node对象
        // 实际停止逻辑在节点的析构函数中处理
        std::cout << "\n接收到关闭信号(SIGINT)，准备停止电机..." << std::endl;
    });

    RCLCPP_INFO(node->get_logger(), "速度订阅节点运行中...");
    RCLCPP_INFO(node->get_logger(), "按Ctrl+C停止节点并停止电机");
    
    try {
        rclcpp::spin(node);
    } catch (const std::exception& e) {
        RCLCPP_ERROR(node->get_logger(), "节点运行异常: %s", e.what());
    } catch (...) {
        RCLCPP_ERROR(node->get_logger(), "未知异常");
    }

    // 节点停止时执行紧急停止
    RCLCPP_INFO(node->get_logger(), "节点停止，执行紧急停止...");
    node->emergencyStop();
    
    // 等待一段时间确保停止消息被发送
    std::this_thread::sleep_for(100ms);
    
    rclcpp::shutdown();
    RCLCPP_INFO(node->get_logger(), "节点已关闭");

    return 0;
}
