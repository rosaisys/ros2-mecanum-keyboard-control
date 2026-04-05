# 话题演示启动文件
# 启动速度发布者和订阅者节点
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        # ROS机器人控制器节点
        Node(
            package='ros_robot_controller',
            executable='ros_robot_controller',
            name='ros_robot_controller',
            output='screen',
            parameters=[{'imu_frame': 'imu_link'}]
        ),

        # 速度订阅者节点
        Node(
            package='smart_car_core_demo',
            executable='speed_subscriber',
            name='speed_subscriber',
            output='screen'
        ),
      
        # 速度发布者节点
        Node(
            package='smart_car_core_demo',
            executable='speed_publisher',
            name='speed_publisher',
            output='screen',
            parameters=[{'publish_frequency': 10.0}]  # 10Hz发布频率
        ),
        
  

    ])
