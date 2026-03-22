#include <chrono>
#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"
// 新增头文件
#include <atomic>
#include <thread>
#include <iostream>
#include <termios.h>
#include <unistd.h>

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
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);
        timer_ = this->create_wall_timer(
            1s, 
            std::bind(&SpeedPublisher::timer_callback, this)
        );
    }

   private:  // ✅ 私有成员，保护封装性
    void timer_callback() {
        auto message = geometry_msgs::msg::Twist();
        message.linear.x = 0.3;  // 设置线速度为0.1 m/s
        message.linear.y = 0.0; 
        message.linear.z = 0.0;
        message.angular.x = 0.0;
        message.angular.y = 0.0;
        message.angular.z = 0.0; // 设置角速度为0.0 rad/s
        publisher_->publish(message);

        // 打印日志
        RCLCPP_INFO(this->get_logger(),
                    "发布车速度指令：线速度=%.2f m/s, 角速度=%.2f rad/s",
                    message.linear.x, message.angular.z);
    }
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char *argv[]){
    rclcpp::init(argc,argv);
    rclcpp::spin(std::make_shared<SpeedPublisher>());
    rclcpp::shutdown();
    return 0;
}
