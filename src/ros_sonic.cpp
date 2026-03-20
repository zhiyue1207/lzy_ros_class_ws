#include <ros/ros.h> 
#include <sensor_msgs/Range.h> 
// 左超声波传感器回调 
void rangeCallback1(const sensor_msgs::Range::ConstPtr& msg)
{
	 ROS_INFO("Distance Left: %f m", msg->range); 
	}
// 前超声波传感器回调 
void rangeCallback2(const sensor_msgs::Range::ConstPtr& msg)
{
	 ROS_INFO("Distance Front: %f m", msg->range); 
	 }
 // 右超声波传感器回调 
 void rangeCallback3(const sensor_msgs::Range::ConstPtr& msg)
 { 
 	ROS_INFO("Distance Right: %f m", msg->range); 
 	}
 int main(int argc, char** argv)
 { 
 	ros::init(argc, argv, "ultrasonic_subscriber"); 
 	ros::NodeHandle nh; // 订阅超声波传感器话题
 	ros::Subscriber sub_1 = nh.subscribe<sensor_msgs::Range>("/ul/sensor1", 10, rangeCallback1); 	    
 	ros::Subscriber sub_2 = nh.subscribe<sensor_msgs::Range>("/ul/sensor2", 10, rangeCallback2); 	 
 	ros::Subscriber sub_3 = nh.subscribe<sensor_msgs::Range>("/ul/sensor3", 10, rangeCallback3); 	
 	
 	ros::spin(); 
 	return 0; 
 }
