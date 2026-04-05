#ifndef SMART_CAR_CORE_DEMO__KEYBOARD_INPUT_HPP_
#define SMART_CAR_CORE_DEMO__KEYBOARD_INPUT_HPP_

#include <string>
#include <mutex>
#include <atomic>

// 平台特定的键盘输入头文件
#ifdef _WIN32
#include <conio.h>  // Windows: _kbhit(), _getch()
#include <windows.h>
#else
#include <sys/select.h>  // Linux/Mac: 非阻塞键盘检测
#include <termios.h>     // Linux/Mac: 终端模式控制
#include <unistd.h>      // Unix系统调用
#endif
#include <cstring>
#include <iostream>
#include <csignal>

/**
 * @brief 跨平台键盘输入检测类
 * 
 * 功能：提供非阻塞的键盘输入检测，支持普通按键和转义序列（如箭头键）
 * 特点：
 *   - 单例模式，确保全局只有一个实例
 *   - 自动管理终端设置，程序退出时自动恢复
 *   - 支持信号处理，确保异常退出时也能恢复终端
 *   - 线程安全设计
 *   - 全局退出标志，支持优雅退出
 */
class KeyboardInput {
public:
    /**
     * @brief 获取单例实例
     * @return KeyboardInput& 单例实例引用
     */
    static KeyboardInput &getInstance();

    /**
     * @brief 获取按键输入（非阻塞）
     * 
     * @return std::string 按下的键字符，无按键时返回空字符串
     * 
     * Windows实现：使用_kbhit()和_getch()
     * Linux/Mac实现：使用select()进行非阻塞检测
     * 支持读取转义序列（如箭头键）
     */
    std::string getKey() const;

    /**
     * @brief 恢复终端设置（可在信号处理函数中调用）
     * 
     * 注意：此方法在Linux/Mac系统上有效，Windows系统无需特殊处理
     */
    void restoreTerminal();

    /**
     * @brief 检查是否应该退出
     * @return bool 如果应该退出返回true，否则返回false
     */
    static bool shouldExit();

    /**
     * @brief 请求退出
     */
    static void requestExit();

    // 禁止拷贝构造和赋值
    KeyboardInput(const KeyboardInput &) = delete;
    KeyboardInput &operator=(const KeyboardInput &) = delete;

private:
    /**
     * @brief 构造函数：初始化终端设置
     * 
     * Linux/Mac系统：保存原始终端设置，配置为无缓冲、无回显模式
     * Windows系统：无需特殊初始化
     */
    KeyboardInput();

    /**
     * @brief 析构函数：恢复原始终端设置
     */
    ~KeyboardInput();

    /**
     * @brief 信号处理函数（静态方法）
     * @param signal 接收到的信号编号
     */
    static void signalHandler(int signal);

#ifndef _WIN32
    struct termios     original_termios_;   // Linux/Mac: 原始终端设置
    mutable std::mutex mutex_;              // 保护终端恢复操作
    bool               terminal_restored_;  // 标记终端是否已恢复
#endif
};

#endif  // SMART_CAR_CORE_DEMO__KEYBOARD_INPUT_HPP_
