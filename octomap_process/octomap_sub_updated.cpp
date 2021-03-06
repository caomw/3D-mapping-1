#include <iostream>
#include "ros/ros.h"
#include "sensor_msgs/PointCloud2.h"
#include <octomap/octomap.h>
#include <octomap/OcTree.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/conversions.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/PCLPointCloud2.h>
#include <pcl/filters/filter.h>
#include <octomap_msgs/conversions.h>
#include <octomap_msgs/Octomap.h>
#include <deque>
#include <pcl/octree/octree.h>

std::deque< pcl::PointCloud<pcl::PointXYZ> > cloud_seq_loaded; 
const float res = 0.1;






void processCloud(const sensor_msgs::PointCloud2 msg)
{
	//********* Retirive and process raw pointcloud************
	std::cout<<"Recieved cloud"<<std::endl;
	std::cout<<"Create Octomap"<<std::endl;
	//octomap::OcTree tree(res);
	std::cout<<"Load points "<<std::endl;
	pcl::PCLPointCloud2 cloud;
	pcl_conversions::toPCL(msg,cloud);
	pcl::PointCloud<pcl::PointXYZ> pcl_pc;
	pcl::fromPCLPointCloud2(cloud,pcl_pc);
	std::cout<<"Filter point clouds for NAN"<<std::endl;
	std::vector<int> nan_indices;
	pcl::removeNaNFromPointCloud(pcl_pc,pcl_pc,nan_indices);
	//octomap::Pointcloud oct_pc;
	//octomap::point3d origin(0.0f,0.0f,0.0f);
	std::cout<<"Adding point cloud to octomap"<<std::endl;
	//octomap::point3d origin(0.0f,0.0f,0.0f);
	//for(int i = 0;i<pcl_pc.points.size();i++){
		//oct_pc.push_back((float) pcl_pc.points[i].x,(float) pcl_pc.points[i].y,(float) pcl_pc.points[i].z);
	//}
	//tree.insertPointCloud(oct_pc,origin,-1,false,false);
	
	//*********** Remove the oldest data, update the data***************	
	cloud_seq_loaded.push_back(pcl_pc);
	std::cout<<cloud_seq_loaded.size()<<std::endl;
	if(cloud_seq_loaded.size()>2){
		cloud_seq_loaded.pop_front();
	
	}
	
	//*********** Process currently observerd and buffered data*********

	if(cloud_seq_loaded.size()==2){
		std::cout<< "Generating octomap"<<std::endl;
		
		pcl::PointCloud<pcl::PointXYZ>::Ptr prev_pc (new pcl::PointCloud<pcl::PointXYZ>); 
		*prev_pc = cloud_seq_loaded.front();
		pcl::PointCloud<pcl::PointXYZ>::Ptr curr_pc (new pcl::PointCloud<pcl::PointXYZ>);
		*curr_pc =pcl_pc;
		pcl::octree::OctreePointCloudChangeDetector<pcl::PointXYZ> octree (32.0f);
		octree.setInputCloud(prev_pc);
		octree.addPointsFromInputCloud();
		octree.switchBuffers();
		octree.setInputCloud(curr_pc);
		octree.addPointsFromInputCloud();
		std::vector<int> newPointIdxVector;
		// Get vector of point indices from octree voxels which did not exist in previous buffer
		octree.getPointIndicesFromNewVoxels (newPointIdxVector);
		std::cout << "Output from getPointIndicesFromNewVoxels:" << std::endl;
		for (size_t i = 0; i < newPointIdxVector.size (); ++i){
				std::cout << i << "# Index:" << newPointIdxVector[i]<< "  Point:" << pcl_pc.points[newPointIdxVector[i]].x << " "<< pcl_pc.points[newPointIdxVector[i]].y << " "<< pcl_pc.points[newPointIdxVector[i]].z << std::endl;
			//std::cout<< curr_coord<<std::endl;
		}	
			//std::cout << "Node center: " << it.getCoordinate() << std::endl;
			//std::cout << "Node size: " << it.getSize() << std::endl;
			//std::cout << "Node value: " << it->getValue() << std::endl;
				

		}
		//********** print out the statistics ******************
		
	
	//**************Process Point cloud in octree data structure *****************
	
	
	
	
	
	/*
	//******************Traverse the tree ********************
	for(octomap::OcTree::tree_iterator it =tree.begin_tree(), end = tree.end_tree();it!= end;it++){
		 //manipulate node, e.g.:
		std::cout << "_____________________________________"<<std::endl;
		std::cout << "Node center: " << it.getCoordinate() << std::endl;
		std::cout << "Node size: " << it.getSize() << std::endl;
		std::cout << "Node depth: "<<it.getDepth() << std::endl;
		std::cout << "Is Leaf : "<< it.isLeaf()<< std::endl;
		std::cout << "_____________________________________"<<std::endl;
		}
	//**********************************************************	
	*/
	std::cout<<"finished"<<std::endl;
	std::cout<<std::endl;
	}


int main(int argc, char **argv){	
	std::cout<<std::endl;
	std::cout<<"initializing ROS node"<<std::endl;
	ros::init(argc,argv, "processCloud");
	ros::NodeHandle n;
	std::cout<<"subscribing to topic point_cloud"<<std::endl;
	//std::string topic =n.resolveName("point_cloud");
	uint32_t queue_size = 1;
	
	ros::Subscriber sub = n.subscribe<sensor_msgs::PointCloud2>("/camera/depth/points",queue_size,processCloud);
	//std::cout<<"Publishing OCtree as a msg"<<std::endl;
	//ros::Publisher oct_pub = n.advertise<octomap_msgs::Octomap>("oct_msg",1);
	//oct_pub.publish(map_msg);
	
	//if there are sufficient number of observations publish them.
 	
	ros::spin(); 
	return 0;
}
