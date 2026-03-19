# lzy_ros_class_ws
本仓库包含课堂实验 ROS 工程，包含话题通信、服务通信、Action 动作库及多传感器避障功能。

## 一、环境依赖
- ROS Noetic
- Ubuntu 20.04
- 依赖库：actionlib, actionlib_msgs，std_msgs

## 二、快速启动指南

### 1. 克隆代码仓库
```bash
git clone https://github.com/zhiyue1207/lzy_ros_class_ws.git ~/lzy_ros_class_ws

cd ~/lzy_ros_class_ws
catkin_make
source devel/setup.bash

roscore

# 启动普通发布者
rosrun my_class_pkg ros_publisher_node

# 启动消息发布者
rosrun my_class_pkg msg_publisher_node

# 启动普通订阅者
rosrun my_class_pkg ros_subscriber_node

# 启动消息订阅者
rosrun my_class_pkg msg_subscriber_node

# 启动服务端
rosrun my_class_pkg ros_server_node

# 启动客户端 (新终端)
rosrun my_class_pkg ros_client_node

# 避障相关节点
rosrun my_class_pkg ros_bump_avoid_node
rosrun my_class_pkg ros_bump_node

# IMU 惯性测量单元节点
rosrun my_class_pkg ros_imu_node
rosrun my_class_pkg ros_imu_rotate_node

# TOF 测距避障节点
rosrun my_class_pkg ros_tof_avoid_node
rosrun my_class_pkg ros_tof_node

# 超声波传感器节点
rosrun my_class_pkg ros_sonic_node

# 避障相关节点
rosrun my_class_pkg ros_bump_avoid_node
rosrun my_class_pkg ros_bump_node

# IMU 惯性测量单元节点
rosrun my_class_pkg ros_imu_node
rosrun my_class_pkg ros_imu_rotate_node

# TOF 测距避障节点
rosrun my_class_pkg ros_tof_avoid_node
rosrun my_class_pkg ros_tof_node

# 超声波传感器节点
rosrun my_class_pkg ros_sonic_node

