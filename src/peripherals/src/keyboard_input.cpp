#include "smart_car_core_demo/keyboard_input.hpp"

// 键盘检测参数
#define KEYBOARD_TIMEOUT_US 50000  // 键盘检测超时时间 (微秒) = 50ms

// 全局退出标志
namespace {
std::atomic<bool> g_should_exit{false};
}

/**
 * @brief 获取单例实例
 */
KeyboardInput &KeyboardInput::getInstance() {
    static KeyboardInput instance;
    return instance;
}

/**
 * @brief 检查是否应该退出
 */
bool KeyboardInput::shouldExit() { 
    return g_should_exit.load(); 
}

/**
 * @brief 请求退出
 */
void KeyboardInput::requestExit() { 
    g_should_exit.store(true); 
}

/**
 * @brief 获取按键输入（非阻塞）
 *
 * @return std::string 按下的键字符，无按键时返回空字符串
 *
 * Windows实现：使用_kbhit()和_getch()
 * Linux/Mac实现：使用select()进行非阻塞检测
 * 支持读取转义序列（如箭头键）
 */
std::string KeyboardInput::getKey() const {
#ifdef _WIN32
    // Windows系统：检测键盘输入
    if (_kbhit()) {
        char ch = _getch();
        // 如果是转义字符，继续读取后续字符
        if (ch == '\x1b') {
            if (_kbhit()) {
                ch = _getch();
                if (ch == '[') {
                    if (_kbhit()) {
                        ch = _getch();
                        return std::string("\x1b[") + ch;  // 返回完整的转义序列
                    }
                }
            }
        }
        return std::string(1, ch);
    }
    return "";
#else
    // Linux/Mac系统：使用select进行非阻塞检测
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    struct timeval timeout;
    timeout.tv_sec  = 0;
    timeout.tv_usec = KEYBOARD_TIMEOUT_US;  // 使用宏定义的键盘检测超时时间

    int retval      = select(STDIN_FILENO + 1, &readfds, nullptr, nullptr, &timeout);

    if (retval == -1) {
        // 如果select被信号中断，忽略错误继续运行
        if (errno == EINTR) {
            return "";
        }
        std::cerr << "select() error: " << strerror(errno) << std::endl;
        return "";
    } else if (retval > 0) {
        char buffer[10];
        int bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer));
        if (bytes_read > 0) {
            return std::string(buffer, bytes_read);
        }
    }

    return "";
#endif
}

/**
 * @brief 恢复终端设置（可在信号处理函数中调用）
 */
void KeyboardInput::restoreTerminal() {
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

/**
 * @brief 构造函数：初始化终端设置
 *
 * Linux/Mac系统：保存原始终端设置，配置为无缓冲、无回显模式
 * Windows系统：无需特殊初始化
 */
KeyboardInput::KeyboardInput() {
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
KeyboardInput::~KeyboardInput() { 
    restoreTerminal(); 
}

/**
 * @brief 信号处理函数（静态方法）
 */
void KeyboardInput::signalHandler(int signal) {
    std::cout << "\n接收到信号 " << signal << "，设置退出标志并恢复终端设置..." << std::endl;
    // 设置全局退出标志
    requestExit();
    getInstance().restoreTerminal();
    // 重新注册默认信号处理（可选，但不再重新发送信号）
    std::signal(signal, SIG_DFL);
}
