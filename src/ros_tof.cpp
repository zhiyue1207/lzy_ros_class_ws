#include <ros/ros.h> 
#include <sensor_msgs/Range.h> 
// 左TOF传感器回调 
void tofCallback1(const sensor_msgs::Range::ConstPtr& msg)
{ 
	ROS_INFO("TOF Distance Left: %f m", msg->range); 
} 
// 前TOF传感器回调 
void tofCallback2(const sensor_msgs::Range::ConstPtr& msg)
{ 
	ROS_INFO("TOF Distance Front: %f m", msg->range);
 } 
 // 右TOF传感器回调 
 void tofCallback3(const sensor_msgs::Range::ConstPtr& msg)
 { 
 	ROS_INFO("TOF Distance Right: %f m", msg->range); 
 } 
 int main(int argc, char** argv)
 { 
 	ros::init(argc, argv, "tof_subscriber"); 
 	ros::NodeHandle nh; // 订阅TOF传感器话题 
 	ros::Subscriber sub_1 = nh.subscribe<sensor_msgs::Range>("/us/tof1", 10, tofCallback1); 
 	ros::Subscriber sub_2 = nh.subscribe<sensor_msgs::Range>("/us/tof2", 10, tofCallback2);
  	ros::Subscriber sub_3 = nh.subscribe<sensor_msgs::Range>("/us/tof3", 10, tofCallback3); 
  
  	ros::spin(); 
  	return 0; 
  }
