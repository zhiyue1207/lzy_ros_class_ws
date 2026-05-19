#include <ros/ros.h>
#include <sensor_msgs/Range.h>
#include <geometry_msgs/Twist.h> 
  ros::Publisher vel_pub; 
  geometry_msgs::Twist cmd_vel;
   const float SAFE_DIST = 0.3; // 安全距离0.3米，可根据实际调整 
   float left_dist = 10.0, front_dist = 10.0, right_dist = 10.0; // 初始化距离为远距 
   // 左TOF回调 
   void tofLeftCallback(const sensor_msgs::Range::ConstPtr& msg)
   { 
   right_dist = msg->range;
    } 
    // 前TOF回调 
    void tofFrontCallback(const sensor_msgs::Range::ConstPtr& msg)
    { 
    front_dist = msg->range; 
    } 
    // 右TOF回调 
    void tofRightCallback(const sensor_msgs::Range::ConstPtr& msg)
    { 
    left_dist = msg->range; 
    } 
    // 避障逻辑处理函数（定时执行，10Hz）
     void avoidLoop(const ros::TimerEvent& event)
      { 
      cmd_vel.linear.x = 0.0; 
      cmd_vel.angular.z = 0.0; 
      // 判断各方向距离是否小于安全阈值 
      if (front_dist < SAFE_DIST && front_dist > 0.0) // 正前方有障碍（排除无效值） 
      { 
      ROS_WARN("Front Obstacle! Distance: %f m, Backward!", front_dist); 
      cmd_vel.linear.x=-0.15;//
      cmd_vel.angular.z = -0.3; // 右转 
      } 
      else if (left_dist < SAFE_DIST && left_dist > 0.0) // 左前方有障碍 
      { 
      ROS_WARN("Left Obstacle! Distance: %f m, Turn Right!", left_dist); 
      cmd_vel.angular.z = -0.2; 
      } 
      else if (right_dist < SAFE_DIST && right_dist > 0.0) // 右前方有障碍 
      { 
      ROS_WARN("Right Obstacle! Distance: %f m, Turn Left!", right_dist); 
      cmd_vel.angular.z = 0.2;
       } 
       else // 无障碍，直行 
       { ROS_INFO("No Obstacle, Move Forward! Dist: L=%.2f F=%.2f R=%.2f", left_dist, front_dist, right_dist); 
         cmd_vel.linear.x = 0.15; // 直行速度 
       } 
       vel_pub.publish(cmd_vel); 
       } 
       
       int main(int argc, char** argv)
       { 
       ros::init(argc, argv, "tof_avoid_node"); 
       ros::NodeHandle nh;
       
        // 发布运动控制话题 
       vel_pub = nh.advertise<geometry_msgs::Twist>("/cmd_vel", 10); 
       // 订阅TOF传感器话题 
       ros::Subscriber sub_left = nh.subscribe<sensor_msgs::Range>("/us/tof1", 10, tofLeftCallback); 
       ros::Subscriber sub_front = nh.subscribe<sensor_msgs::Range>("/us/tof2", 10, tofFrontCallback); 
       ros::Subscriber sub_right = nh.subscribe<sensor_msgs::Range>("/us/tof3", 10, tofRightCallback); 
       // 定时执行避障逻辑，10Hz 
       ros::Timer timer = nh.createTimer(ros::Duration(0.1), avoidLoop); 
       
       ros::spin(); 
       return 0; 
       }
