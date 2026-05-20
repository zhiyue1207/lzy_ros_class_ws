#include <ros/ros.h>
#include <sensor_msgs/Imu.h>
#include <tf2/utils.h>
#include <geometry_msgs/Twist.h>
#include <algorithm>
#include <angles/angles.h>

// 全局变量：当前航向角
double current_yaw = 0.0;

void imu_callback(const sensor_msgs::Imu::ConstPtr& imu_msg) {
    // 从四元数解算航向角 yaw
    current_yaw = tf2::getYaw(imu_msg->orientation);
    // 可选：打印角度（调试用）
    ROS_INFO("当前航向角: %.2f rad", current_yaw);
}

int main(int argc, char** argv) {
    ros::init(argc, argv, "imu_rotate_180");
    ros::NodeHandle nh;

    // 订阅 IMU
    ros::Subscriber imu_sub = nh.subscribe("/imu/data", 10, imu_callback);
    // 发布速度指令
    ros::Publisher vel_pub = nh.advertise<geometry_msgs::Twist>("/cmd_vel", 10);

    // 等待 IMU 数据到达（必须等，否则角度无效）
    ROS_INFO("等待 IMU 数据...");
    ros::Rate wait_rate(10);
    while (ros::ok() && current_yaw == 0.0) {
        ros::spinOnce();
        wait_rate.sleep();
    }
    ROS_INFO("IMU 已就绪，开始自转 180°");

    // 记录起始角度
    double start_yaw = current_yaw;
    bool finished = false;
    ros::Rate rate(20);  // 控制频率 20Hz

    while (ros::ok() && !finished) {
        geometry_msgs::Twist cmd_vel;

        // 计算到目标角度的最短距离
        double target_yaw = start_yaw + M_PI;
        double error = angles::shortest_angular_distance(current_yaw, target_yaw);
        ROS_INFO("角度误差: %.2f", error);

        // 闭环控制：还没到就继续转
        if (fabs(error) > 0.08) {  // 误差大于 0.08rad 继续旋转
            cmd_vel.linear.x = 0.0;
            cmd_vel.angular.z = 0.4;  // 旋转速度，可调整
        } else {  // 到达目标，停止
            cmd_vel.angular.z = 0.0;
            finished = true;
            ROS_INFO("✅ 旋转 180° 完成，已停止！");
        }

        vel_pub.publish(cmd_vel);
        ros::spinOnce();
        rate.sleep();
    }

    // 最终确保停止
    geometry_msgs::Twist stop_msg;
    stop_msg.angular.z = 0.0;
    vel_pub.publish(stop_msg);

    return 0;
}
