#include <atomic>
#include <chrono>
#include <cmath>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"
#include "speed_control_msgs/srv/adjust_speed.hpp"
#include "std_srvs/srv/set_bool.hpp"

// 平台特定的键盘输入头文件
#ifdef _WIN32
#include <conio.h>  // Windows: _kbhit(), _getch()
#include <windows.h>
#else
#include <sys/select.h>  // Linux/Mac: 非阻塞键盘检测
#include <termios.h>     // Linux/Mac: 终端模式控制
#include <unistd.h>      // Unix系统调用
#endif

using namespace std::chrono_literals;

// ============================================================================
// 关键参数配置区 - 所有重要参数集中在此处，方便修改
// ============================================================================

// 1. 基础速度参数（根据机器人类型自动选择）
#define DEFAULT_LINEAR_VELOCITY 0.2   // 默认线速度 (m/s)
#define DEFAULT_ANGULAR_VELOCITY 0.5  // 默认角速度 (rad/s)

// 2. 超快响应参数（竞技级配置）
#define ACCELERATION_LIMIT 2.5          // 线加速度限制 (m/s²) - 原0.5 → 2.5（5倍！）
#define ANGULAR_ACCELERATION_LIMIT 4.0  // 角加速度限制 (rad/s²) - 原1.0 → 4.0（4倍！）

// 3. 控制频率参数
#define CONTROL_FREQUENCY 100  // 控制频率 (Hz)
#define CONTROL_PERIOD_MS 10   // 控制周期 (ms) = 1000 / CONTROL_FREQUENCY
#define CONTROL_DT 0.01        // 时间步长 (秒) = 1.0 / CONTROL_FREQUENCY

// 4. 停止阈值参数（超快响应：更低的停止阈值）
#define STOP_THRESHOLD_LINEAR 0.002   // 线速度停止阈值 - 原0.01 → 0.002（5倍精度！）
#define STOP_THRESHOLD_ANGULAR 0.005  // 角速度停止阈值 - 原0.01 → 0.005（2倍精度！）

// 5. 发布阈值参数（超快响应：更敏感的发布阈值）
#define PUBLISH_THRESHOLD_LINEAR 0.0001   // 线速度发布阈值 - 原0.001 → 0.0001（10倍敏感！）
#define PUBLISH_THRESHOLD_ANGULAR 0.0002  // 角速度发布阈值 - 原0.001 → 0.0002（5倍敏感！）

// 6. 键盘检测参数
#define KEYBOARD_TIMEOUT_US 50000  // 键盘检测超时时间 (微秒) = 50ms
#define IDLE_COUNT_THRESHOLD 5     // 空闲计数阈值（阿克曼机器人）

// 7. 机器人类型配置
#define ROBOT_TYPE_DIFFERENTIAL ""             // 差速驱动机器人（默认）
#define ROBOT_TYPE_ACKERMANN "JetRover_Acker"  // 阿克曼转向机器人

// ============================================================================
// 参数使用说明：
// 1. 修改加速度参数：调整 ACCELERATION_LIMIT 和 ANGULAR_ACCELERATION_LIMIT
// 2. 修改响应速度：调整 STOP_THRESHOLD_* 和 PUBLISH_THRESHOLD_*
// 3. 修改控制频率：调整 CONTROL_FREQUENCY 和 CONTROL_DT
// 4. 修改基础速度：调整 DEFAULT_LINEAR_VELOCITY 和 DEFAULT_ANGULAR_VELOCITY
// ============================================================================
/**
 * @brief 跨平台键盘输入工具类（改进版）
 *
 * 功能：提供Windows/Linux/Mac系统的非阻塞键盘输入检测
 * 设计原则：RAII资源管理，自动恢复终端设置
 * 改进点：
 *   1. 添加信号处理，确保程序异常退出时恢复终端设置
 *   2. 使用单例模式确保终端设置只被修改一次
 *   3. 添加线程安全保护
 * ROS2通信：无直接ROS2依赖，纯输入工具类
 */
// 全局退出标志，用于信号处理
namespace {
std::atomic<bool> g_should_exit{false};
}

class KeyboardInput {
   public:
    /**
     * @brief 获取单例实例
     */
    static KeyboardInput &getInstance() {
        static KeyboardInput instance;
        return instance;
    }

    /**
     * @brief 检查是否应该退出
     */
    static bool shouldExit() { return g_should_exit.load(); }

    /**
     * @brief 请求退出
     */
    static void requestExit() { g_should_exit.store(true); }

    /**
     * @brief 获取按键输入（非阻塞）
     *
     * @return std::string 按下的键字符，无按键时返回空字符串
     *
     * Windows实现：使用_kbhit()和_getch()
     * Linux/Mac实现：使用select()进行非阻塞检测
     */
    std::string getKey() const {
#ifdef _WIN32
        // Windows系统：检测键盘输入
        if (_kbhit()) {
            char ch = _getch();
            return std::string(1, ch);
        }
        return "";
#else
        // Linux/Mac系统：使用select进行非阻塞检测
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);

        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = KEYBOARD_TIMEOUT_US;  // 使用宏定义的键盘检测超时时间

        int retval = select(STDIN_FILENO + 1, &readfds, nullptr, nullptr, &timeout);

        if (retval == -1) {
            // 如果select被信号中断，忽略错误继续运行
            if (errno == EINTR) {
                return "";
            }
            std::cerr << "select() error: " << strerror(errno) << std::endl;
            return "";
        } else if (retval > 0) {
            char ch;
            if (read(STDIN_FILENO, &ch, 1) == 1) {
                return std::string(1, ch);
            }
        }

        return "";
#endif
    }

    /**
     * @brief 恢复终端设置（可在信号处理函数中调用）
     */
    void restoreTerminal() {
#ifndef _WIN32
        std::lock_guard<std::mutex> lock(mutex_);
        if (terminal_restored_) {
            return;
        }
        tcsetattr(STDIN_FILENO, TCSANOW, &original_termios_);
        terminal_restored_ = true;
        std::cout << "\n终端设置已恢复" << std::endl;
#endif
    }

   private:
    /**
     * @brief 构造函数：初始化终端设置
     *
     * Linux/Mac系统：保存原始终端设置，配置为无缓冲、无回显模式
     * Windows系统：无需特殊初始化
     */
    KeyboardInput() {
#ifndef _WIN32
        // 保存原始终端设置
        tcgetattr(STDIN_FILENO, &original_termios_);

        // 配置新终端设置：关闭行缓冲和回显
        struct termios new_termios = original_termios_;
        new_termios.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);

        // 注册信号处理函数，确保程序退出时恢复终端
        std::signal(SIGINT, signalHandler);
        std::signal(SIGTERM, signalHandler);
        std::signal(SIGSEGV, signalHandler);
        std::signal(SIGABRT, signalHandler);

        terminal_restored_ = false;
#endif
    }

    /**
     * @brief 析构函数：恢复原始终端设置
     */
    ~KeyboardInput() { restoreTerminal(); }

    // 禁止拷贝构造和赋值
    KeyboardInput(const KeyboardInput &) = delete;
    KeyboardInput &operator=(const KeyboardInput &) = delete;

    /**
     * @brief 信号处理函数（静态方法）
     */
    static void signalHandler(int signal) {
        std::cout << "\n接收到信号 " << signal << "，设置退出标志并恢复终端设置..." << std::endl;
        // 设置全局退出标志
        requestExit();
        getInstance().restoreTerminal();
        // 重新注册默认信号处理（可选，但不再重新发送信号）
        std::signal(signal, SIG_DFL);
    }

#ifndef _WIN32
    struct termios original_termios_;  // Linux/Mac: 原始终端设置
    mutable std::mutex mutex_;         // 保护终端恢复操作
    bool terminal_restored_;           // 标记终端是否已恢复
#endif
};

/**
 * @brief 智能车速度控制节点
 *
 * 功能：通过键盘控制智能车运动，发布速度指令到/cmd_vel话题
 * ROS2通信机制：
 *   - 发布者：geometry_msgs::msg::Twist 类型，话题名 "cmd_vel"
 *   - QoS设置：默认10个消息的队列深度
 *   - 节点类型：rclcpp::Node
 *
 * 支持两种机器人类型：
 *   1. 麦克纳姆轮/差速驱动机器人：直接速度控制
 *   2. 阿克曼转向机器人：转向角度计算
 */
class SpeedPublisher : public rclcpp::Node {
   public:
    /**
     * @brief 构造函数：初始化ROS2节点和参数
     *
     * @param robot_name 机器人名称，用于日志标识
     */
    explicit SpeedPublisher(const std::string &robot_name = "smart_car")
        : Node("speed_publisher"),
          control_linear_vel_(0.0),
          control_angular_vel_(0.0),
          last_published_linear_(0.0),
          last_published_angular_(0.0),
          idle_count_(0),
          should_stop_(false),
          smooth_mode_(true) {
        // 参数声明
        this->declare_parameter<std::string>("command_topic", "cmd_vel");
        this->declare_parameter<std::string>("control_mode_service", "set_control_mode");
        this->declare_parameter<std::string>("robot_type", "");
        this->declare_parameter<int>("queue_depth", 10);
        this->declare_parameter<double>("publish_rate", 100.0);
        this->declare_parameter<double>("default_linear_velocity", 0.2);
        this->declare_parameter<double>("default_angular_velocity", 0.5);
        this->declare_parameter<double>("acceleration_limit", 2.5);
        this->declare_parameter<double>("angular_acceleration_limit", 4.0);
        this->declare_parameter<double>("ackermann_wheelbase", 0.213);
        this->declare_parameter<double>("ackermann_steering_angle", 0.628);
        this->declare_parameter<double>("stop_threshold_linear", 0.002);
        this->declare_parameter<double>("stop_threshold_angular", 0.005);
        this->declare_parameter<double>("publish_threshold_linear", 0.0001);
        this->declare_parameter<double>("publish_threshold_angular", 0.0002);

        // 参数获取
        this->get_parameter("command_topic", command_topic_);
        this->get_parameter("control_mode_service", control_mode_service_);
        this->get_parameter("robot_type", machine_type_);
        this->get_parameter("queue_depth", queue_depth_);
        this->get_parameter("publish_rate", publish_rate_);
        this->get_parameter("default_linear_velocity", default_linear_velocity_);
        this->get_parameter("default_angular_velocity", default_angular_velocity_);
        this->get_parameter("acceleration_limit", acceleration_limit_);
        this->get_parameter("angular_acceleration_limit", angular_acceleration_limit_);
        this->get_parameter("ackermann_wheelbase", ackermann_wheelbase_);
        this->get_parameter("ackermann_steering_angle", ackermann_steering_angle_);
        this->get_parameter("stop_threshold_linear", stop_threshold_linear_);
        this->get_parameter("stop_threshold_angular", stop_threshold_angular_);
        this->get_parameter("publish_threshold_linear", publish_threshold_linear_);
        this->get_parameter("publish_threshold_angular", publish_threshold_angular_);

        if (machine_type_.empty()) {
            const char *machine_type_env = std::getenv("MACHINE_TYPE");
            machine_type_ = machine_type_env ? std::string(machine_type_env) : "";
        }

        if (publish_rate_ <= 0.0) {
            publish_rate_ = 100.0;
        }

        control_period_ms_ = std::max(1, static_cast<int>(std::lround(1000.0 / publish_rate_)));
        control_dt_ = static_cast<double>(control_period_ms_) / 1000.0;

        // 初始化ROS2发布者：发布到/cmd_vel话题
        publisher_ =
            this->create_publisher<geometry_msgs::msg::Twist>(command_topic_, static_cast<size_t>(queue_depth_));

        set_control_mode_service_ = this->create_service<std_srvs::srv::SetBool>(
            control_mode_service_, [this](const std::shared_ptr<std_srvs::srv::SetBool::Request> request,
                                          std::shared_ptr<std_srvs::srv::SetBool::Response> response) {
                this->setControlModeCallback(request, response);
            });

        // 根据机器类型配置速度参数
        configureSpeedParameters();

        RCLCPP_INFO(this->get_logger(), "速度控制节点已启动，机器人类型: %s, 名称: %s, 话题: %s", machine_type_.c_str(),
                    robot_name.c_str(), command_topic_.c_str());

        // 显示控制说明
        printControlInstructions();
    }

    /**
     * @brief 运行主控制循环（改进版）
     *
     * 功能：持续检测键盘输入，根据按键更新速度指令并发布
     * 改进点：
     *   1. 使用单例KeyboardInput，避免多次修改终端设置
     *   2. 添加速度平滑处理，避免突变
     *   3. 改进退出逻辑，确保资源正确释放
     * 控制逻辑：
     *   - W/S键：控制前进/后退线速度
     *   - A/D键：控制左转/右转角速度
     *   - CTRL-C：退出控制循环
     *   - 无按键：根据机器类型执行不同的停止逻辑
     */
    void runControlLoop() {
        auto &keyboard = KeyboardInput::getInstance();

        RCLCPP_INFO(this->get_logger(), "开始键盘控制，按CTRL-C退出...");

        // 速度平滑处理变量
        double target_linear_vel_ = 0.0;
        double target_angular_vel_ = 0.0;
        // 使用ROS参数配置的超快响应参数
        const double acceleration = acceleration_limit_;
        const double angular_acceleration = angular_acceleration_limit_;

        while (rclcpp::ok() && !should_stop_ && !KeyboardInput::shouldExit()) {
            std::string key = keyboard.getKey();

            // 处理键盘输入，更新目标速度
            processKeyInput(key, target_linear_vel_, target_angular_vel_);
            if (smooth_mode_) {
                // 速度平滑处理：逐步接近目标速度
                smoothVelocity(target_linear_vel_, target_angular_vel_, acceleration, angular_acceleration);
            } else {
                // 原始暴躁模式：直接设置目标速度（无平滑处理）
                control_linear_vel_ = target_linear_vel_;
                control_angular_vel_ = target_angular_vel_;
            }

            // 发布速度指令（仅在速度变化时发布，减少网络负载）
            publishVelocityIfChanged();

            // 控制循环频率：使用宏定义的控制周期
            std::this_thread::sleep_for(std::chrono::milliseconds(control_period_ms_));
        }
    }

    /**
     * @brief 根据机器类型配置速度参数
     */
    void configureSpeedParameters() {
        if (machine_type_ == "JetRover_Acker") {
            // 阿克曼转向机器人参数
            linear_velocity_ = default_linear_velocity_;
            // 阿克曼转向角速度计算：v = ω * R，R = L / tan(δ)
            angular_velocity_ = linear_velocity_ / (ackermann_wheelbase_ / std::tan(ackermann_steering_angle_));
            RCLCPP_INFO(this->get_logger(), "配置为阿克曼转向模式，线速度: %.2f m/s, 角速度: %.2f rad/s",
                        linear_velocity_, angular_velocity_);
        } else {
            // 麦克纳姆轮/差速驱动机器人参数（默认）
            linear_velocity_ = default_linear_velocity_;
            angular_velocity_ = default_angular_velocity_;
            RCLCPP_INFO(this->get_logger(), "配置为差速驱动模式，线速度: %.2f m/s, 角速度: %.2f rad/s",
                        linear_velocity_, angular_velocity_);
        }
    }

    /**
     * @brief 设置控制模式服务回调函数
     *
     * @param request 请求：data为true表示启用平滑模式，false表示原始模式
     * @param response 响应：success表示设置成功，message为状态信息
     */
    void setControlModeCallback(const std::shared_ptr<std_srvs::srv::SetBool::Request> request,
                                std::shared_ptr<std_srvs::srv::SetBool::Response> response) {
        try {
            smooth_mode_ = request->data;
            response->success = true;
            if (smooth_mode_) {
                response->message = "已切换到平滑模式（平稳起步）";
                RCLCPP_INFO(this->get_logger(), "控制模式切换：平滑模式");
            } else {
                response->message = "已切换到原始暴躁模式（直接输出键盘速度）";
                RCLCPP_INFO(this->get_logger(), "控制模式切换：原始暴躁模式");
            }
        } catch (const std::exception &e) {
            response->success = false;
            response->message = "模式切换失败：" + std::string(e.what());
            RCLCPP_ERROR(this->get_logger(), "异常：%s", e.what());
        }
    }

    /**
     * @brief 设置速度参数服务回调函数
     *
     * @param request 请求：linear_velocity和angular_velocity为新的速度参数值
     * @param response 响应：success表示设置成功，message为状态信息
     * @param current_linear 响应：当前线速度参数值
     * @param current_angular 响应：当前角速度参数值
     */
    void adjustSpeedCallback(const std::shared_ptr<speed_control_msgs::srv::AdjustSpeed::Request> request,
                             std::shared_ptr<speed_control_msgs::srv::AdjustSpeed::Response> response) {
        if (request->direction == 1) {
            linear_velocity_ += request->step;
            angular_velocity_ += request->step;
        } else if (request->direction == -1) {
            linear_velocity_ -= request->step;
            angular_velocity_ -= request->step;
        } else if (request->direction == 0) {
            linear_velocity_ = DEFAULT_LINEAR_VELOCITY;
            angular_velocity_ = DEFAULT_ANGULAR_VELOCITY;
        }

        // 确保速度在合理范围内
        linear_velocity_ = std::max(0.1, std::min(linear_velocity_, 1.0));
        angular_velocity_ = std::max(0.1, std::min(angular_velocity_, 1.0));

        response->success = true;
        response->message = "速度参数已调整";
        response->current_linear = linear_velocity_;
        response->current_angular = angular_velocity_;
    }

    /**
     * @brief 处理键盘输入（改进版）
     *
     * @param key 按下的键字符
     * @param target_linear_vel 目标线速度（输出参数）
     * @param target_angular_vel 目标角速度（输出参数）
     */
    void processKeyInput(const std::string &key, double &target_linear_vel, double &target_angular_vel) {
        if (key.empty()) {
            // 无按键时的处理逻辑
            handleNoKeyInput(target_linear_vel, target_angular_vel);
            return;
        }

        if (key == "\x03") {  // CTRL-C
            should_stop_ = true;
            return;
        }

        // 重置空闲计数器
        idle_count_ = 0;

        if (machine_type_ == "JetRover_Acker") {
            // 阿克曼机器人控制逻辑
            handleAckermannControl(key, target_linear_vel, target_angular_vel);
        } else {
            // 差速驱动机器人控制逻辑
            handleDifferentialControl(key, target_linear_vel, target_angular_vel);
        }
    }

    /**
     * @brief 处理差速驱动机器人控制（改进版）
     *
     * @param key 按下的键字符
     * @param target_linear_vel 目标线速度（输出参数）
     * @param target_angular_vel 目标角速度（输出参数）
     */
    void handleDifferentialControl(const std::string &key, double &target_linear_vel, double &target_angular_vel) {
        if (key == "w") {
            target_linear_vel = linear_velocity_;
            target_angular_vel = 0.0;
        } else if (key == "s") {
            target_linear_vel = -linear_velocity_;
            target_angular_vel = 0.0;
        } else if (key == "a") {
            target_angular_vel = angular_velocity_;
            target_linear_vel = 0.0;
        } else if (key == "d") {
            target_angular_vel = -angular_velocity_;
            target_linear_vel = 0.0;
        } else if (key == " ") {  // 空格键：紧急停止
            target_linear_vel = 0.0;
            target_angular_vel = 0.0;
            control_linear_vel_ = 0.0;
            control_angular_vel_ = 0.0;
            RCLCPP_WARN(this->get_logger(), "紧急停止！");
        }
    }

    /**
     * @brief 处理阿克曼机器人控制（改进版）
     *
     * @param key 按下的键字符
     * @param target_linear_vel 目标线速度（输出参数）
     * @param target_angular_vel 目标角速度（输出参数）
     */
    void handleAckermannControl(const std::string &key, double &target_linear_vel, double &target_angular_vel) {
        if (key == "w") {
            target_linear_vel = linear_velocity_;
        } else if (key == "s") {
            target_linear_vel = -linear_velocity_;
        } else if (key == "a") {
            target_angular_vel = angular_velocity_;
        } else if (key == "d") {
            target_angular_vel = -angular_velocity_;
        } else if (key == " ") {  // 空格键：紧急停止
            target_linear_vel = 0.0;
            target_angular_vel = 0.0;
            control_linear_vel_ = 0.0;
            control_angular_vel_ = 0.0;
            RCLCPP_WARN(this->get_logger(), "紧急停止！");
        }
    }

    /**
     * @brief 处理无按键输入情况（改进版）
     *
     * @param target_linear_vel 目标线速度（输出参数）
     * @param target_angular_vel 目标角速度（输出参数）
     */
    void handleNoKeyInput(double &target_linear_vel, double &target_angular_vel) {
        if (machine_type_ == "JetRover_Acker") {
            // 阿克曼机器人：空闲计数，达到阈值后停止
            idle_count_++;
            if (idle_count_ > IDLE_COUNT_THRESHOLD) {
                target_linear_vel = 0.0;
                target_angular_vel = 0.0;
                idle_count_ = 0;
            }
        } else {
            // 差速驱动机器人：立即停止旋转，保持线速度
            target_angular_vel = 0.0;
            target_linear_vel = 0.0;
            // 线速度保持不变，实现惯性滑行
        }
    }
}

    /**
     * @brief 速度平滑处理函数
     *
     * @param target_linear_vel 目标线速度
     * @param target_angular_vel 目标角速度
     * @param acceleration 线加速度限制 (m/s²)
     * @param angular_acceleration 角加速度限制 (rad/s²)
     */
    void smoothVelocity(double target_linear_vel, double target_angular_vel, double acceleration,
                        double angular_acceleration) {
    // 使用宏定义的时间步长
    const double dt = control_dt_;

    // 线速度平滑处理
    double linear_diff = target_linear_vel - control_linear_vel_;
    double max_linear_change = acceleration * dt;

    if (std::abs(linear_diff) > max_linear_change) {
        control_linear_vel_ += (linear_diff > 0 ? max_linear_change : -max_linear_change);
    } else {
        control_linear_vel_ = target_linear_vel;
    }

    // 角速度平滑处理
    double angular_diff = target_angular_vel - control_angular_vel_;
    double max_angular_change = angular_acceleration * dt;

    if (std::abs(angular_diff) > max_angular_change) {
        control_angular_vel_ += (angular_diff > 0 ? max_angular_change : -max_angular_change);
    } else {
        control_angular_vel_ = target_angular_vel;
    }
}

/**
 * @brief 平滑停止函数
 *
 * @param acceleration 线减速度限制 (m/s²)
 * @param angular_acceleration 角减速度限制 (rad/s²)
 */
void smoothStop(double acceleration, double angular_acceleration) {
    RCLCPP_INFO(this->get_logger(), "执行平滑停止...");

    // 逐步减速到零（使用宏定义的停止阈值）
    while ((std::abs(control_linear_vel_) > stop_threshold_linear_ ||
            std::abs(control_angular_vel_) > stop_threshold_angular_) &&
           rclcpp::ok()) {
        smoothVelocity(0.0, 0.0, acceleration, angular_acceleration);
        publishVelocityIfChanged();
        std::this_thread::sleep_for(std::chrono::milliseconds(control_period_ms_));
    }

    RCLCPP_INFO(this->get_logger(), "平滑停止完成");
}

/**
 * @brief 发布速度指令（仅在速度变化时发布）
 */
void publishVelocityIfChanged() {
    // 检查速度是否有变化（使用宏定义的发布阈值）
    bool linear_changed = std::abs(control_linear_vel_ - last_published_linear_) > publish_threshold_linear_;
    bool angular_changed = std::abs(control_angular_vel_ - last_published_angular_) > publish_threshold_angular_;

    if (linear_changed || angular_changed || control_angular_vel_ != 0.0) {
        auto twist = std::make_unique<geometry_msgs::msg::Twist>();
        twist->linear.x = control_linear_vel_;
        twist->linear.y = 0.0;
        twist->linear.z = 0.0;
        twist->angular.x = 0.0;
        twist->angular.y = 0.0;
        twist->angular.z = control_angular_vel_;

        publisher_->publish(*twist);

        // 更新最后发布的速度值
        last_published_linear_ = control_linear_vel_;
        last_published_angular_ = control_angular_vel_;

        // 显示当前速度
        std::cout << "\r[控制指令] 线速度: " << std::fixed << std::setprecision(2) << twist->linear.x
                  << " m/s | 角速度: " << twist->angular.z << " rad/s        " << std::flush;
    }
}

/**
 * @brief 发布停止指令
 */
void publishStopCommand() {
    auto stop_twist = std::make_unique<geometry_msgs::msg::Twist>();
    stop_twist->linear.x = 0.0;
    stop_twist->linear.y = 0.0;
    stop_twist->linear.z = 0.0;
    stop_twist->angular.x = 0.0;
    stop_twist->angular.y = 0.0;
    stop_twist->angular.z = 0.0;

    publisher_->publish(*stop_twist);
    last_published_linear_ = 0.0;
    last_published_angular_ = 0.0;
    RCLCPP_INFO(this->get_logger(), "已发送停止指令");
}

/**
 * @brief 显示控制说明（改进版）
 */
void printControlInstructions() {
    std::cout << "\n=============================================\n";
    std::cout << "        智能车键盘控制说明（改进版）\n";
    std::cout << "=============================================\n";
    std::cout << "移动方向：\n";
    std::cout << "            w (前进)\n";
    std::cout << "    a (左转)   s (后退)   d (右转)\n";
    std::cout << "\n特殊按键：\n";
    std::cout << "    空格键 : 紧急停止\n";
    std::cout << "    CTRL-C : 退出控制程序\n";
    std::cout << "\n改进特性：\n";
    std::cout << "    • 速度平滑：避免速度突变\n";
    std::cout << "    • 紧急停止：按空格键立即停止\n";
    std::cout << "    • 终端恢复：程序异常退出时自动恢复终端设置\n";
    std::cout << "=============================================\n\n";
}

// ROS2发布者：用于发布速度指令
rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;

// 服务对象
rclcpp::Service<std_srvs::srv::SetBool>::SharedPtr set_control_mode_service_;

// 机器类型和速度参数
std::string machine_type_;
std::string command_topic_;
std::string control_mode_service_;
int queue_depth_;
double publish_rate_;
double default_linear_velocity_;
double default_angular_velocity_;
double acceleration_limit_;
double angular_acceleration_limit_;
double ackermann_wheelbase_;
double ackermann_steering_angle_;
double stop_threshold_linear_;
double stop_threshold_angular_;
double publish_threshold_linear_;
double publish_threshold_angular_;
int control_period_ms_;
double control_dt_;
double linear_velocity_;
double angular_velocity_;

// 控制变量
double control_linear_vel_;
double control_angular_vel_;
double last_published_linear_;
double last_published_angular_;
int idle_count_;

// 控制标志
std::atomic<bool> should_stop_;

// 控制模式：true为平滑模式，false为原始模式
bool smooth_mode_;
}
;

/**
 * @brief 主函数：ROS2节点入口点
 *
 * 功能：
 *   1. 初始化ROS2上下文
 *   2. 创建速度控制节点
 *   3. 在单独线程中运行控制循环
 *   4. 处理ROS2回调
 *   5. 清理资源
 */
int main(int argc, char *argv[]) {
    // 初始化ROS2
    rclcpp::init(argc, argv);

    // 获取机器人名称（从环境变量或使用默认值）
    const char *host_env = std::getenv("HOST");
    std::string robot_name = host_env ? std::string(host_env) : "smart_car";

    RCLCPP_INFO(rclcpp::get_logger("main"), "启动智能车速度控制节点，机器人: %s", robot_name.c_str());

    try {
        // 创建速度控制节点（使用智能指针管理）
        auto speed_node = std::make_shared<SpeedPublisher>(robot_name);

        // 在单独线程中运行控制循环
        std::thread control_thread([speed_node]() { speed_node->runControlLoop(); });

        // 主线程：处理ROS2回调（如果有的话）
        rclcpp::spin(speed_node);

        // ROS2 spin返回后，说明ROS2上下文正在关闭
        RCLCPP_INFO(rclcpp::get_logger("main"), "ROS2上下文正在关闭，请求停止控制循环...");

        // 请求停止控制循环
        speed_node->stop();

        // 同时设置全局退出标志，确保控制循环能够检测到
        KeyboardInput::requestExit();

        // 等待控制线程结束
        if (control_thread.joinable()) {
            control_thread.join();
        }

        RCLCPP_INFO(rclcpp::get_logger("main"), "控制节点正常关闭");

    } catch (const std::exception &e) {
        RCLCPP_ERROR(rclcpp::get_logger("main"), "控制节点异常: %s", e.what());
        // 确保设置退出标志
        KeyboardInput::requestExit();
        return 1;
    }

    // 清理ROS2资源
    rclcpp::shutdown();
    return 0;
}
