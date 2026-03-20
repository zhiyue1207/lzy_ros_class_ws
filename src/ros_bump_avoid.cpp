#include "ros/ros.h"
#include "std_msgs/Int16MultiArray.h" 
#include "geometry_msgs/Twist.h"
  ros::Publisher vel_pub; // 运动控制发布者 
  geometry_msgs::Twist cmd_vel; // 运动控制消息 
  // 碰撞传感器回调函数 
  void bumpAvoidCallback(const std_msgs::Int16MultiArray::ConstPtr& msg)
   { 
   // 初始化运动指令为停止 
   cmd_vel.linear.x = 0.0; 
   cmd_vel.angular.z = 0.0; 
   // 判断碰撞传感器状态 
   int left_front = msg->data[0]; // 0号传感器：左前 
   int front = msg->data[1]; // 1号传感器：正前 
   int right_front = msg->data[2]; // 2号传感器：右前 
   if (front == 1) // 正前方碰撞：后退+随机左转 
   { 
   	ROS_WARN("Front Collision! Back and Turn Left!"); 
   	cmd_vel.linear.x = -0.1; // 后退（线速度x负方向） 
   	cmd_vel.angular.z = 0.2; // 左转（角速度z正方向）
    } 
    else if (left_front == 1) // 左前方碰撞：后退+右转 
    { 
    	ROS_WARN("Left Front Collision! Back and Turn Right!"); 
    	cmd_vel.linear.x = -0.1; 
    	cmd_vel.angular.z = -0.2; 
    } 
    else if (right_front == 1) // 右前方碰撞：后退+左转 
    { 
    	ROS_WARN("Right Front Collision! Back and Turn Left!"); 
    	cmd_vel.linear.x = -0.1; 
    	cmd_vel.angular.z = 0.2; 
    } 
    else // 无碰撞直行）
     {
      ROS_INFO("No Collision, Moving Forward!"); 
      cmd_vel.linear.x=0.1;
      cmd_vel.angular.z=0;
     } 
     // 发布运动控制指令 
     vel_pub.publish(cmd_vel); 
     } 
     
     int main(int argc, char **argv) 
     { 
     	ros::init(argc, argv, "bump_avoid_node"); 
     	ros::NodeHandle n; 
     // 发布/cmd_vel话题，队列大小10 
     	vel_pub = n.advertise<geometry_msgs::Twist>("/cmd_vel", 10); 
     // 订阅碰撞传感器话题 
     	ros::Subscriber sub = n.subscribe("/robot/bump_sensor", 1000, bumpAvoidCallback); 
     	
     	ros::spin(); 
     	return 0; 
     }
