#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <tf2/LinearMath/Quaternion.h>
#include <std_msgs/String.h>
#include <apriltag_ros/AprilTagDetectionArray.h>
#include <vector>

using namespace std;

// Global variable: stores detected AprilTag IDs
vector<int> detected_tags;
// Define navigation client
typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

// ---------------------- Dynamic parameter config helper ----------------------
void setYawTolerance(double tolerance)
{
    ros::NodeHandle nh;
    // Set yaw goal tolerance for local planner (works for both DWA and TEB)
    nh.setParam("/move_base/DWAPlannerROS/yaw_goal_tolerance", tolerance);
    nh.setParam("/move_base/TebLocalPlannerROS/yaw_goal_tolerance", tolerance);
    ROS_INFO("Set yaw goal tolerance to %.2f rad", tolerance);
}

// ---------------------- AprilTag Detection Callback ----------------------
void tagCallback(const apriltag_ros::AprilTagDetectionArray::ConstPtr& msg)
{
    detected_tags.clear();
    for (auto& detection : msg->detections) {
        for (int id : detection.id) {
            detected_tags.push_back(id);
            ROS_DEBUG("Detected AprilTag ID: %d", id);
        }
    }
}

// ---------------------- Voice Broadcast & Tag Detection ----------------------
void checkAndSpeak(ros::Publisher& pub, int target_id)
{
    // Wait for stable detection (increased to 1s for reliability)
    ros::WallDuration(1.0).sleep();
    ros::spinOnce();

    std_msgs::String voice;
    bool found = false;
    for (int id : detected_tags) {
        if (id == target_id) {
            found = true;
            break;
        }
    }

    if (found) {
        voice.data = "已找到目标一";
        ROS_INFO("Target found: AprilTag ID=%d", target_id);
    } else {
        voice.data = "未找到目标一";
        ROS_WARN("Target not found: Looking for AprilTag ID=%d", target_id);
        // Print all detected IDs for debugging
        if (detected_tags.empty()) {
            ROS_WARN("No AprilTags detected at all");
        } else {
            string detected_str;
            for (int id : detected_tags) detected_str += to_string(id) + " ";
            ROS_WARN("Detected IDs: %s", detected_str.c_str());
        }
    }
    pub.publish(voice);
    ros::WallDuration(1.5).sleep(); // Wait for voice broadcast to finish
}

// ---------------------- Main Function ----------------------
int main(int argc, char** argv)
{
    ros::init(argc, argv, "navi_tag_voice_node");
    ros::NodeHandle nh;

    // Async spinner (4 threads) to prevent blocking
    ros::AsyncSpinner spinner(4);
    spinner.start();

    // Initialize voice publisher and tag subscriber
    // ✅ FIXED: Correct topic name for W2A robot: /tag_detections
    ros::Publisher voice_pub = nh.advertise<std_msgs::String>("/talk", 10);
    ros::Subscriber tag_sub = nh.subscribe("/tag_detections", 10, tagCallback);

    // Connect to navigation server
    MoveBaseClient ac("move_base", true);
    ROS_INFO("Waiting for move_base server...");
    ac.waitForServer();
    ROS_INFO("✅ Navigation server connected");

    move_base_msgs::MoveBaseGoal goal1, goal2, goal3, home;
    tf2::Quaternion q;

    // ====================== Goal 1 (Need specific angle for AprilTag) ======================
    setYawTolerance(0.1); // Strict angle tolerance for alignment
    q.setRPY(0, 0, -1.5707);
    goal1.target_pose.pose.position.x = 2.600991;
    goal1.target_pose.pose.position.y = -1.611283;
    goal1.target_pose.pose.orientation.z = q.z();
    goal1.target_pose.pose.orientation.w = q.w();
    goal1.target_pose.header.frame_id = "map";
    goal1.target_pose.header.stamp = ros::Time::now();

    ac.sendGoal(goal1);
    ac.waitForResult();
    if (ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED) {
        ROS_INFO("Reached Goal 1, detecting ID=1");
        checkAndSpeak(voice_pub, 1);
    } else {
        ROS_WARN("Failed to reach Goal 1");
    }

    // ====================== Goal 2 (Need specific angle for AprilTag) ======================
    setYawTolerance(0.1); // Keep strict tolerance
    q.setRPY(0, 0, 1.5707);
    goal2.target_pose.pose.position.x = 2.923071;
    goal2.target_pose.pose.position.y = -3.627572;
    goal2.target_pose.pose.orientation.z = q.z();
    goal2.target_pose.pose.orientation.w = q.w();
    goal2.target_pose.header.frame_id = "map";
    goal2.target_pose.header.stamp = ros::Time::now();

    ac.sendGoal(goal2);
    ac.waitForResult();
    if (ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED) {
        ROS_INFO("Reached Goal 2, detecting ID=1");
        checkAndSpeak(voice_pub, 1);
    } else {
        ROS_WARN("Failed to reach Goal 1");
    }

    // ====================== Goal 3 (NO ANGLE REQUIREMENT) ======================
    setYawTolerance(3.14); // ✅ Allow ±180° error: completely ignore orientation
    goal3.target_pose.pose.position.x = 1.226119;
    goal3.target_pose.pose.position.y = -3.395227;
    // Use default orientation (doesn't matter anymore)
    q.setRPY(0, 0, 0);
    goal3.target_pose.pose.orientation.z = q.z();
    goal3.target_pose.pose.orientation.w = q.w();
    goal3.target_pose.header.frame_id = "map";
    goal3.target_pose.header.stamp = ros::Time::now();

    ac.sendGoal(goal3);
    ac.waitForResult();
    ROS_INFO("✅ Reached Goal 3 (skipped rotation)");

    // ====================== Home (NO ANGLE REQUIREMENT) ======================
    setYawTolerance(3.14); // Keep ignoring orientation
    home.target_pose.pose.position.x = -0.435562;
    home.target_pose.pose.position.y = -2.640729;
    q.setRPY(0, 0, 0);
    home.target_pose.pose.orientation.z = q.z();
    home.target_pose.pose.orientation.w = q.w();
    home.target_pose.header.frame_id = "map";
    home.target_pose.header.stamp = ros::Time::now();

    ac.sendGoal(home);
    ac.waitForResult();
    ROS_INFO("✅ Returned to starting point");

    // Reset tolerance to default for future use
    setYawTolerance(0.1);

    ros::waitForShutdown();
    return 0;
}
