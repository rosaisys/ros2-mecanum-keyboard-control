#include <memory>
#include <cmath>
#include <vector>
#include <functional>
#include <algorithm>

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "ros_robot_controller_msgs/msg/motor_state.hpp"
#include "ros_robot_controller_msgs/msg/motors_state.hpp"

using namespace std::chrono_literals;

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
    double speedToRps(double speed) const {
        return speed / (M_PI * wheel_diameter_);
    }

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
        std::vector<double> speeds = {
            speedToRps(motor1),
            speedToRps(motor2),
            speedToRps(-motor3),  // 注意：电机3和4方向相反
            speedToRps(-motor4)
        };
        
        // 创建MotorsState消息
        ros_robot_controller_msgs::msg::MotorsState motors_msg;
        
        for (size_t i = 0; i < speeds.size(); ++i) {
            auto motor_state = ros_robot_controller_msgs::msg::MotorState();
            motor_state.id = static_cast<uint16_t>(i + 1);  // 电机ID从1开始
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
            "cmd_vel", 10,
            [this](const geometry_msgs::msg::Twist::SharedPtr msg) {
                this->cmdVelCallback(msg);
            });
        
        // 创建发布者，发布电机速度到ros_robot_controller/set_motor话题
        motor_publisher_ = this->create_publisher<ros_robot_controller_msgs::msg::MotorsState>(
            "ros_robot_controller/set_motor", 10);
        
        RCLCPP_INFO(this->get_logger(), "速度订阅节点已启动，等待cmd_vel指令...");
    }

private:
    /**
     * @brief cmd_vel话题回调函数
     * @param msg 速度指令消息
     */
    void cmdVelCallback(const geometry_msgs::msg::Twist::SharedPtr msg) {
        // 限制速度范围，确保安全
        double linear_x = msg->linear.x;
        double linear_y = msg->linear.y;
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
                   linear_x, linear_y, angular_z);
        
        RCLCPP_INFO(this->get_logger(),
                   "发布电机转速: 1:%.2f, 2:%.2f, 3:%.2f, 4:%.2f rps",
                   motors_msg.data[0].rps, motors_msg.data[1].rps,
                   motors_msg.data[2].rps, motors_msg.data[3].rps);
    }
    
    // 使用智能指针管理订阅者和发布者
    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_subscription_;
    rclcpp::Publisher<ros_robot_controller_msgs::msg::MotorsState>::SharedPtr motor_publisher_;
    
    // 运动学计算器
    MecanumChassis chassis_;
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    
    // 使用智能指针创建节点
    auto node = std::make_shared<SpeedSubscriber>();
    
    // 使用lambda函数处理信号
    auto signal_handler = [&node](int) {
        RCLCPP_INFO(node->get_logger(), "接收到关闭信号，停止电机...");
        
        // 发布停止指令
        auto stop_msg = ros_robot_controller_msgs::msg::MotorsState();
        for (int i = 1; i <= 4; ++i) {
            auto motor_state = ros_robot_controller_msgs::msg::MotorState();
            motor_state.id = i;
            motor_state.rps = 0.0;
            stop_msg.data.push_back(motor_state);
        }
        
        node->create_publisher<ros_robot_controller_msgs::msg::MotorsState>(
            "ros_robot_controller/set_motor", 10)->publish(stop_msg);
        
        rclcpp::shutdown();
    };
    
    // 设置信号处理（在实际ROS2应用中可能需要其他方式）
    // 这里主要是展示lambda函数的使用
    
    RCLCPP_INFO(node->get_logger(), "速度订阅节点运行中...");
    rclcpp::spin(node);
    rclcpp::shutdown();
    
    return 0;
}
