# 话题演示启动文件
# 统一从 params.yaml 读取参数

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    params_file = LaunchConfiguration('params_file')
    params_file_argument = DeclareLaunchArgument(
        'params_file',
        default_value=PathJoinSubstitution([
            FindPackageShare('smart_car_core_demo'),
            'config',
            'params.yaml',
        ]),
        description='Path to the shared parameters file',
    )

    return LaunchDescription([
        params_file_argument,
        Node(
            package='ros_robot_controller',
            executable='ros_robot_controller',
            name='ros_robot_controller',
            output='screen',
            parameters=[params_file],
        ),
        Node(
            package='smart_car_core_demo',
            executable='speed_subscriber',
            name='speed_subscriber',
            output='screen',
            parameters=[params_file],
        ),
        Node(
            package='smart_car_core_demo',
            executable='speed_publisher',
            name='speed_publisher',
            output='screen',
            parameters=[params_file],
        ),
    ])
