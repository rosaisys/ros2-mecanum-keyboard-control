# 智能车话题示例启动文件
# 启动车速发布和订阅节点

from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        # 车速发布节点
        Node(
            package='smart_car_core_demo',
            executable='speed_publisher',
            name='speed_publisher',
            output='screen'
        ),
        
        # 车速订阅节点（简化版）
        Node(
            package='smart_car_core_demo',
            executable='speed_subscriber_simple',
            name='speed_subscriber_simple',
            output='screen'
        ),
    ])
