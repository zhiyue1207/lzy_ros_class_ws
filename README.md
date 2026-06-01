
# lzy_ros_class_ws

科创24 ROS课程实验工作空间，适配智行-W2A机器人，包含第1～7周所有实验代码、节点与launch文件。

---

## 环境准备

```bash
# 安装依赖
sudo apt update
sudo apt install ros-noetic-* ros-noetic-dynamic-reconfigure ros-noetic-cv-bridge ros-noetic-navigation
pip3 install opencv-python mediapipe apriltag openai

# 拉取代码并编译
git clone https://github.com/zhiyue1207/lzy_ros_class_ws.git
cd lzy_ros_class_ws
catkin_make

# 配置环境变量（仅首次需要）
echo "source ~/lzy_ros_class_ws/devel/setup.bash" >> ~/.bashrc
source ~/.bashrc
```

---

## 第1周 底盘驱动与主题消息实验

### 1. 底盘与里程计

```bash
# 启动机器人硬件
roslaunch upros_bringup bringup_w2a.launch

# 键盘控制底盘
rosrun upros_move_linear teleop_twist_keyboard.py

# 查看里程计（rviz）
rviz

# 开环前进节点
rosrun my_class_pkg move_forward

# 里程计闭环控制节点
rosrun my_class_pkg odom_move
```

### 2. 话题与消息通信

```bash
# C++ 发布者
rosrun my_class_publisher_node

# C++ 订阅者
rosrun my_subscriber_node

# Python 发布者
rosrun my_class_pkg ros_publisher_node.py

# Python 订阅者
rosrun my_class_pkg ros_subscriber_node.py

# 自定义消息查看
rosmsg show my_class_pkg/MyMessage

# 一次性启动话题相关节点
roslaunch my_class_pkg bringup_topic.launch
```

---

## 第2周 多传感器避障实验

```bash
# 碰撞传感器避障
rosrun my_class_pkg bump_avoid_node

# 超声/TOF避障
rosrun my_class_pkg us_tof_avoid_node

# ROS 调试工具
rqt_graph
rqt_plot
rqt_console
rqt_image_view
rosrun rqt_tf_tree rqt_tf_tree
```

---

## 第3周 参数与动态参数

```bash
# C++ 参数节点
rosrun my_class_pkg ros_param_node

# Python 参数节点
rosrun my_class_pkg ros_param.py

# Launch 加载参数
roslaunch my_class_pkg parameter.launch

# 动态参数服务端
rosrun my_class_pkg dynamic_reconfigure_node

# 动态参数配置界面
rosrun rqt_reconfigure rqt_reconfigure

# 动态调速节点
rosrun my_class_pkg ros_dynamic_speed_node
```

---

## 第4周 Gazebo 仿真

```bash
# 启动仿真机器人
roslaunch zx_description w2a.launch

# 仿真建图
roslaunch zx_description gmapping.launch

# 键盘控制移动
rosrun teleop_twist_keyboard.py

# 保存地图
roslaunch zx_description save_map.launch

# 仿真导航
roslaunch zx_description navigation.launch
```

---

## 第5周 机器视觉实验

```bash
# 深度相机 + OpenCV 图像处理
rosrun my_class_pkg get_ros_image.py

# HSV 视觉巡线
rosrun my_class_pkg follow_line.py
rostopic pub -1 /enable_move std_msgs/Int16 "data: 1"

# MediaPipe 手势控制
rosrun my_class_pkg gesture_movement.py

# AprilTag 视觉跟踪
rosrun my_class_pkg apriltag_follow.py

# AprilTag 机械臂抓取
roslaunch upros_arm recognize_apriltag.launch
rosrun my_class_pkg tag_grab_node
```

---

## 第6周 IMU + 激光雷达 + SLAM

### IMU

```bash
# 订阅 IMU 数据
rosrun my_class_pkg ros_imu_node

# IMU 自旋转 180°
rosrun my_class_pkg ros_imu_rotate_node
```

### 激光雷达

```bash
# 查看雷达数据
rosrun my_class_pkg ros_scan_node

# 激光雷达避障
rosrun my_class_pkg ros_avoid_node
```

### SLAM 建图与导航

```bash
# 启动硬件
roslaunch upros_bringup bringup_w2a.launch

# 建图
roslaunch upros_navigation gmapping.launch
rosrun teleop_twist_keyboard.py
roslaunch upros_navigation save_map.launch

# 自主导航
roslaunch upros_navigation navigation.launch
roslaunch upros_navigation view_nav.launch

# 定点巡航
rosrun my_class_pkg movebase_client_node
```

---

## 第7周 语音交互与大模型

```bash
# 离线语音识别
roslaunch upros_chat speech_to_word.launch
rostopic echo /speech/result

# 大模型问答（需配置 moonshot_api_key.txt）
rosrun my_class_pkg llm_chat.py

# 离线语音合成
roslaunch upros_chat word_to_speech.launch
rostopic pub -1 /talk std_msgs/String "data: 你好，我是智行机器人"

# 语音指令控制机械臂抓取
rosrun my_class_pkg tokenizer.py
rosrun my_class_pkg voice_control.py
```

---

## 通用说明

- 新开终端需执行：
```bash
source ~/lzy_ros_class_ws/devel/setup.bash
```
- 真机实验前必须启动硬件：
```bash
roslaunch upros_bringup bringup_w2a.launch
```
- 每周提交示例：
```bash
git checkout -b weekX
git add .
git commit -m "完成第X周 ROS 实验"
git push origin weekX
```
```
