#include <atomic>
#include <chrono>
#include <csignal>
#include <cstring>
#include <functional>  //存放「函数包装器、回调函数、绑定器」等
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "rclcpp/rclcpp.hpp"
#include "speed_control_msgs/srv/adjust_speed.hpp"
#include "std_srvs/srv/set_bool.hpp"

// 键盘检测参数
#define KEYBOARD_TIMEOUT_US 50000  // 键盘检测超时时间 (微秒) = 50ms

// 全局退出标志，用于信号处理
namespace {
std::atomic<bool> g_should_exit{false};
}

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
class KeyboardInput {
   public:
    /**
     * @brief 获取单例实例
     */
    static KeyboardInput& getInstance() {
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
     *         对于方向键等特殊键，返回完整的转义序列（如"\x1b[A"）
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
            // 读取第一个字符
            char ch;
            if (read(STDIN_FILENO, &ch, 1) == 1) {
                std::string result(1, ch);

                // 如果是ESC字符（0x1b），可能是转义序列的开始
                if (ch == '\x1b') {
                    // 尝试读取更多字符（最多2个）以检测转义序列
                    struct timeval quick_timeout;
                    quick_timeout.tv_sec = 0;
                    quick_timeout.tv_usec = 10000;  // 10ms超时

                    // 读取第二个字符
                    FD_ZERO(&readfds);
                    FD_SET(STDIN_FILENO, &readfds);
                    if (select(STDIN_FILENO + 1, &readfds, nullptr, nullptr, &quick_timeout) > 0) {
                        char ch2;
                        if (read(STDIN_FILENO, &ch2, 1) == 1) {
                            result += ch2;

                            // 如果是'['，继续读取第三个字符
                            if (ch2 == '[') {
                                // 读取第三个字符
                                FD_ZERO(&readfds);
                                FD_SET(STDIN_FILENO, &readfds);
                                if (select(STDIN_FILENO + 1, &readfds, nullptr, nullptr, &quick_timeout) > 0) {
                                    char ch3;
                                    if (read(STDIN_FILENO, &ch3, 1) == 1) {
                                        result += ch3;
                                    }
                                }
                            }
                        }
                    }
                }
                return result;
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
    KeyboardInput(const KeyboardInput&) = delete;
    KeyboardInput& operator=(const KeyboardInput&) = delete;

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
 * @brief 控制模式切换客户端节点
 *
 * 功能：调用set_control_mode服务来切换小车的控制模式
 *   - true: 平滑模式（平稳起步）
 *   - false: 原始暴躁模式（直接输出键盘速度）
 */

class ControlModeClient : public rclcpp::Node {
   private:
    rclcpp::Client<std_srvs::srv::SetBool>::SharedPtr client_;
    std::string service_name_;

   public:
    ControlModeClient() : Node("control_mode_client") {
        this->declare_parameter<std::string>("service_name", "set_control_mode");
        this->get_parameter("service_name", service_name_);
        client_ = this->create_client<std_srvs::srv::SetBool>(service_name_);
    }
    /**
     * @brief 异步调用服务切换控制模式
     *
     * @param smooth_mode true为平滑模式，false为原始模式
     */
    void switchMode(bool smooth_mode) {
        while (!client_->wait_for_service(1s)) {
            if (!rclcpp::ok()) {
                RCLCPP_ERROR(this->get_logger(), "等待服务时被中断");
                return;
            }
            RCLCPP_INFO(this->get_logger(), "等待%s服务可用...", service_name_.c_str());
        }

        auto request = std::make_shared<std_srvs::srv::SetBool::Request>();
        request->data = smooth_mode;

        auto result = client_->async_send_request(request);
        if (rclcpp::spin_until_future_complete(this->shared_from_this(), result) == rclcpp::FutureReturnCode::SUCCESS) {
            auto response = result.get();
            if (response->success) {
                RCLCPP_INFO(this->get_logger(), "模式切换成功: %s", response->message.c_str());
            } else {
                RCLCPP_ERROR(this->get_logger(), "模式切换失败: %s", response->message.c_str());
            }
        } else {
            RCLCPP_ERROR(this->get_logger(), "服务调用失败");
        }
    }
};

/**
 * @brief 主函数：演示如何切换控制模式
 */
int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);
    auto client_node = std::make_shared<ControlModeClient>();
    RCLCPP_INFO(client_node->get_logger(), "控制模式切换客户端已启动");
    RCLCPP_INFO(client_node->get_logger(), "按 's' 切换到平滑模式，按 'o' 切换到原始模式，按 'q' 退出");

    while (rclcpp::ok()) {
        std::string key = KeyboardInput::getInstance().getKey();

        if (!key.empty()) {
            if (key == "s") {
                RCLCPP_INFO(client_node->get_logger(), "切换到平滑模式...");
                client_node->switchMode(true);
            } else if (key == "o") {
                RCLCPP_INFO(client_node->get_logger(), "切换到原始暴躁模式（竞速模式）...");
                client_node->switchMode(false);
            } else if (key == "q") {
                RCLCPP_INFO(client_node->get_logger(), "退出程序");
                break;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    rclcpp::shutdown();
    return 0;
}
