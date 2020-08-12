//
// Created by xcy on 2020/8/10.
//

#include <queue>
#include <ros/forwards.h>


#include <tf/transform_broadcaster.h>
#include <tf_conversions/tf_eigen.h>

#include <ros/ros.h>


#include <iostream>

#include <ceres/ceres.h>

#include <Eigen/Core>
#include <Eigen/Dense>

#include <sensor_msgs/PointCloud.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

#include <sensor_msgs/point_cloud_conversion.h>
#include <pcl/io/pcd_io.h>
#include <pcl/registration/icp.h>



#include <geometry_msgs/PointStamped.h>

#include <dlib/svm_threaded.h>
#include <dlib/optimization/max_cost_assignment.h>
#include <tf/transform_listener.h>

#ifndef SRC_TREECENTERLOCALIZATION_H
#define SRC_TREECENTERLOCALIZATION_H

using namespace std;
class TreeCenterLocalization {
private:
    void tree_callback(const sensor_msgs::PointCloud::ConstPtr& landmarkPCL);
    ros::NodeHandle nh_;
    ros::Subscriber landmarkPCL_sub;

    ros::Publisher landmark_cloud_pub;
    ros::Publisher robot_pose_publisher;

    tf::TransformBroadcaster my_br;
    tf::TransformListener listener;

//    tf::StampedTransform velodyne_to_base;
    tf::StampedTransform velodyne_to_map;


    sensor_msgs::PointCloud map_cloud;

    string lidar_name;
    string base_link_name;
    string odom_name;
    string map_name;
    double MaxCorrespondenceDistance, MaximumIterations, setTransformationEpsilon,EuclideanFitnessEpsilon;

    bool firstTrackFlag;

    void addPointsToMap(sensor_msgs::PointCloud pointsToBeAdded, sensor_msgs::PointCloud& map);
    geometry_msgs::Point32 changeFrame(geometry_msgs::Point32 sourcePoint, string sourceFrame, string targetFrame);
    void addOnePtToMap(geometry_msgs::Point32 new_landmark);
    void realTimeTransformPointCloud(const std::string & target_frame, const tf::Transform& net_transform,
                                                             const ros::Time& target_time, const sensor_msgs::PointCloud & cloudIn,
                                                             sensor_msgs::PointCloud & cloudOut) const;
public:
    TreeCenterLocalization(){
        ros::param::get("~laser_name", lidar_name);
        ros::param::get("~base_link_name",base_link_name);
        ros::param::get("~odom_name",odom_name);
        ros::param::get("~map_name",map_name);


        ros::param::get("~MaxCorrespondenceDistance", MaxCorrespondenceDistance);
        ros::param::get("~MaximumIterations",MaximumIterations);
        ros::param::get("~setTransformationEpsilon",setTransformationEpsilon);
        ros::param::get("~EuclideanFitnessEpsilon",EuclideanFitnessEpsilon);
        landmarkPCL_sub = nh_.subscribe("/tree_center", 1, &TreeCenterLocalization::tree_callback, this);

        landmark_cloud_pub = nh_.advertise<sensor_msgs::PointCloud>("discrete_map", 50);
        robot_pose_publisher = nh_.advertise<geometry_msgs::PoseStamped>("robot_pose_pr2", 10);

        sensor_msgs::PointCloud lanmark_cloud;
        map_cloud.header.frame_id = map_name;
        map_cloud.channels.resize(1);
        map_cloud.channels[0].name = "tree_id";
        map_cloud.points.clear();
        map_cloud.channels[0].values.clear();
        firstTrackFlag = true;
    }

};


#endif //SRC_TREECENTERLOCALIZATION_H
