#include <ros/ros.h>
#include <sensor_msgs/NavSatStatus.h>
#include <sensor_msgs/NavSatFix.h>
#include <std_msgs/Bool.h>
#include <tf/transform_broadcaster.h>
#include "sensor_msgs/Imu.h"
#include <nav_msgs/Odometry.h>



std::string name_space;
geometry_msgs::Quaternion imu_msg;


void MsgCallback(const sensor_msgs::Imu imu_msgs )
{
    imu_msg = imu_msgs.orientation;
    return;
}

void Callback(const nav_msgs::OdometryConstPtr& msg)
{
  static tf::TransformBroadcaster br;
  tf::Transform transform;
  transform.setOrigin( tf::Vector3(msg->pose.pose.position.x, msg->pose.pose.position.y, 0.0) );
  tf::Quaternion quat;
  tf::quaternionMsgToTF(imu_msg, quat);
  // the tf::Quaternion has a method to acess roll pitch and yaw
  double roll, pitch, yaw;
  tf::Matrix3x3(quat).getRPY(roll, pitch, yaw);
  // the found angles are written in a geometry_msgs::Vector3
  tf::Quaternion q;
  q.setRPY(0, 0, yaw+1.57);
  transform.setRotation(q);
  br.sendTransform(tf::StampedTransform(transform, ros::Time::now(), "utm_world", name_space));
}

int main(int argc, char** argv){
  ros::init(argc, argv, "gps_tf_broadcaster");
  if (argc != 2){ROS_ERROR("need name_space as argument"); return -1;};
  name_space = argv[1];

  ros::NodeHandle node;
  ros::Subscriber sub = node.subscribe(name_space+"/odom", 10, &Callback);
  ros::Subscriber sub1 = node.subscribe("/imu/data",10, &MsgCallback);
  ros::spin();
  return 0;
};