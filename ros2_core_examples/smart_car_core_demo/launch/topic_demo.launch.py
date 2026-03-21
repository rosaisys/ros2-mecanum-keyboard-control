# 智能车话题示例启动文件
# 启动车速发布节点

from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        # 车速发布节点
        Node(
            package='smart_car_core_demo',
            executable='speed_publisher',
            name='speed_publisher',
            output='screen',
            parameters=[{
                'linear_speed': 0.5,
                'angular_speed': 0.2,
                'publish_rate': 10.0
            }]
        ),
        
        # 可以在这里添加其他节点，如传感器订阅节点等
        # Node(
        #     package='smart_car_core_demo',
        #     executable='sensor_subscriber',
        #     name='sensor_subscriber',
        #     output='screen'
        # ),
    ])
