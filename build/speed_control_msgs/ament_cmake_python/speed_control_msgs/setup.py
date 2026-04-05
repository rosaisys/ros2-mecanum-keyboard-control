from setuptools import find_packages
from setuptools import setup

setup(
    name='speed_control_msgs',
    version='0.0.0',
    packages=find_packages(
        include=('speed_control_msgs', 'speed_control_msgs.*')),
)
