# ========== ros2_wsorincp 工作空间环境配置 ==========
# 1. 加载工作空间的 ROS 2 环境
if [ -f "$(dirname "$0")/install/setup.zsh" ]; then
    source "$(dirname "$0")/install/setup.zsh"
    echo "✅ ros2_wsorincp 工作空间环境加载成功"
fi

# 2. 可选：如果你有自己的第三方库，添加 LD_LIBRARY_PATH
# export LD_LIBRARY_PATH=$(dirname "$0")/src/your_lib/lib:$LD_LIBRARY_PATH

# 3. 可选：添加工作空间专属的别名
alias build_ws='cd $(dirname "$0") && rm -rf build install log && source /opt/ros/humble/setup.zsh && colcon build --parallel-workers 4'
alias cd_ws='cd $(dirname "$0")'