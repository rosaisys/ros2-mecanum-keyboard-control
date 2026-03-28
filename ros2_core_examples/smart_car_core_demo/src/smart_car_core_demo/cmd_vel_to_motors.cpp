#include <memory>
#include <cmath>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "ros_robot_controller_msgs/msg/motors_state.hpp"
#include "ros_robot_controller_msgs/msg/motor_state.hpp"

/**
 * @brief 将cmd_vel速度指令转换为电机控制指令的节点
 * 
 * 功能：订阅/cmd_vel话题，将其转换为ros_robot_controller_msgs/MotorsState消息
 *       并发布到~/set_motor话题，控制智能车电机
 * 
 * 转换逻辑：
 *   对于四轮差速驱动机器人：
 *     - 左前轮(1)和左后轮(3)：速度 = 线速度 - 角速度 * 轮距/2
 *     - 右前轮(2)和右后轮(4)：速度 = 线速度 + 角速度 * 轮距/2
 * 
 * 参数：
 *   - wheel_base: 轮距（左右轮中心距离，单位：米）
 *   - wheel_radius: 车轮半径（单位：米）
 */
class CmdVelToMotors : public rclcpp::Node {
public:
    CmdVelToMotors() : Node("cmd_vel_to_motors") {
        // 声明参数
        this->declare_parameter("wheel_base", 0.2);      // 默认轮距0.2米
        this->declare_parameter("wheel_radius", 0.05);   // 默认车轮半径0.05米
        
        // 获取参数
        wheel_base_ = this->get_parameter("wheel_base").as_double();
        wheel_radius_ = this->get_parameter("wheel_radius").as_double();
        
        RCLCPP_INFO(this->get_logger(), 
                   "cmd_vel转换节点已启动，轮距: %.3f m, 车轮半径: %.3f m",
                   wheel_base_, wheel_radius_);
        
        // 创建订阅者：订阅/cmd_vel话题
        cmd_vel_subscription_ = this->create_subscription<geometry_msgs::msg::Twist>(
            "/cmd_vel", 10,
            std::bind(&CmdVelToMotors::cmdVelCallback, this, std::placeholders::_1));
        
        // 创建发布者：发布到~/set_motor话题
        motors_publisher_ = this->create_publisher<ros_robot_controller_msgs::msg::MotorsState>(
            "~/set_motor", 10);
        
        RCLCPP_INFO(this->get_logger(), "等待/cmd_vel速度指令...");
    }

private:
    /**
     * @brief 处理/cmd_vel消息的回调函数
     * 
     * @param msg 收到的速度指令消息
     */
    void cmdVelCallback(const geometry_msgs::msg::Twist::SharedPtr msg) {
        // 计算四个电机的速度（RPS：转/秒）
        std::vector<double> wheel_speeds = calculateWheelSpeeds(msg->linear.x, msg->angular.z);
        
        // 创建电机控制消息
        auto motors_msg = ros_robot_controller_msgs::msg::MotorsState();
        
        // 设置四个电机的状态
        for (int i = 0; i < 4; i++) {
            auto motor_state = ros_robot_controller_msgs::msg::MotorState();
            motor_state.id = i + 1;  // 电机ID：1-4
            motor_state.rps = wheel_speeds[i];
            motors_msg.data.push_back(motor_state);
        }
        
        // 发布电机控制消息
        motors_publisher_->publish(motors_msg);
        
        // 显示调试信息
        RCLCPP_DEBUG(this->get_logger(),
                    "收到速度指令: 线速度=%.3f m/s, 角速度=%.3f rad/s",
                    msg->linear.x, msg->angular.z);
        RCLCPP_DEBUG(this->get_logger(),
                    "电机速度: 1=%.3f, 2=%.3f, 3=%.3f, 4=%.3f RPS",
