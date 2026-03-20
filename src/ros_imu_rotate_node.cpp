#include <ros/ros.h>
#include <sensor_msgs/Imu.h>
#include <geometry_msgs/Twist.h>
#include <cmath>

ros::Publisher vel_pub;
geometry_msgs::Twist cmd_vel;
float angular_z = 0.0; 
float rotate_angle = 0.0; 
const float TARGET_ANGLE = M_PI; 
const float ROTATE_SPEED = 0.5;
ros::Time last_time; 
bool is_rotating = false;
bool rotation_done = false; // 新增：旋转完成标志（只转一次）

void imuSpinCallback(const sensor_msgs::Imu::ConstPtr& imu_msg) {
    angular_z = imu_msg->angular_velocity.z;
}

void spinLoop(const ros::TimerEvent& event)
{
    //  核心：如果已经完成，直接return，啥也不干
    if (rotation_done) {
        cmd_vel.linear.x = 0.0;
        cmd_vel.angular.z = 0.0;
        vel_pub.publish(cmd_vel);
        return;
    }

    cmd_vel.linear.x = 0.0;
    cmd_vel.angular.z = 0.0;

    if (!is_rotating)
    {
        last_time = ros::Time::now();
        rotate_angle = 0.0;
        is_rotating = true;
        ROS_INFO("Start rotating 180°...");
        cmd_vel.angular.z = -ROTATE_SPEED;
        vel_pub.publish(cmd_vel);
        return;
    }

    ros::Time current_time = ros::Time::now();
    float dt = (current_time - last_time).toSec();
    last_time = current_time;

    rotate_angle += fabs(angular_z) * dt;

    ROS_INFO("Rotated Angle: %.2f rad / Target: %.2f rad", rotate_angle, TARGET_ANGLE);

    if (rotate_angle < TARGET_ANGLE)
    {
        cmd_vel.angular.z = -ROTATE_SPEED;
        vel_pub.publish(cmd_vel);
    }
    else
    {
        ROS_INFO("180° Rotation Complete! Total Angle: %.2f rad", rotate_angle);
        cmd_vel.linear.x = 0.0;
        cmd_vel.angular.z = 0.0;
        vel_pub.publish(cmd_vel);
        is_rotating = false;
        rotation_done = true; //  关键：标记为已完成，之后永远不再执行
        return;
    }
}

int main(int argc, char** argv){
    ros::init(argc, argv, "imu_spin180_node");
    ros::NodeHandle nh;

    vel_pub = nh.advertise<geometry_msgs::Twist>("/cmd_vel", 10);
    ros::Subscriber imu_sub = nh.subscribe<sensor_msgs::Imu>("/imu/data", 10, imuSpinCallback);
    ros::Timer timer = nh.createTimer(ros::Duration(0.02), spinLoop);

    ros::spin();
    return 0;
}

