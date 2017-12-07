//===============================================================================//
// Name			   : andor_main.cpp
// Author(s)	 :  Kourosh.darvish@edu.unige.it
// Affiliation : University of Genova, Italy - dept. DIBRIS
// Version		 : 1.0
// Description : Main program using the AND-OR graph library
//===============================================================================//

#include <iostream>
#include <ros/ros.h>
#include "andor_aograph.h"
#include "std_msgs/String.h"
#include <andor_msgs/andorSRV.h>
#include <andor_msgs/Hyperarc.h>
#include <andor_msgs/Node.h>
#include <boost/shared_ptr.hpp>

#define RST  "\x1B[0m"
#define KBLU  "\x1B[34m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define FBLU(x) KBLU x RST
#define FRED(x) KRED x RST
#define FGRN(x) KGRN x RST
#define BOLD(x) "\x1B[1m" x RST

using namespace std;
vector<shared_ptr<AOgraph>> graphVector;


bool updateANDOR(andor_msgs::andorSRV::Request &req, andor_msgs::andorSRV::Response &res){
	cout<<"andor receive request"<<endl;
	/*! when arrive a request:
	 * 1- check for the graph name, if it is not valid, return and error
	 * 2- if it is empty the solved nodes or hyper-arcs, respond by the feasible nodes and hyper-arcs
	 * 3- if a node or hyper-arc is solved, update the graph, and respond the feasible nodes/hyper-arcs
	 * 4- if the head node is solved, return it is solved
	 */

	// 1- check for the graph name
	int gIndex=1000; //! requested graph index number in graphVector, initialize wit rnd big value
	for (int i=0;i<graphVector.size();i++)
		if(req.graphName==graphVector[i]->gName)
			gIndex=i;

	if (gIndex==1000){
		cout<<FRED("Graph Name:"<<req.graphName<<" Is False, Check Your Graph Name")<<endl;
		return false;
	}
	// graph name is correct
	else
	{
		cout<<200<<endl;
		if(req.solvedNodes.size()>0)
		{
			cout<<201<<endl;
			for (int i=0; i<req.solvedNodes.size();i++)
			{
				cout<<202<<endl;
				graphVector[gIndex]->solveByNameNode(req.solvedNodes[i]);
			}

		}
		if(req.solvedHyperarc.size()>0)
		{
			cout<<203<<endl;
			for (int i=0; i<req.solvedHyperarc.size();i++)
			{
				cout<<204<<endl;
				graphVector[gIndex]->solveByNameHyperarc(req.solvedHyperarc[i]);
			}

		}
		if (graphVector[gIndex]->isGraphSolved()){
			cout<<205<<endl;
			res.graphSolved=true;
		} else
		{
			cout<<206<<endl;
			res.graphSolved=false;
			vector<andor_msgs::Node> feasileNodeVector;
			vector<andor_msgs::Hyperarc> feasileHyperarcVector;
			graphVector[gIndex]->getFeasibleNode(feasileNodeVector);
			graphVector[gIndex]->getFeasibleHyperarc(feasileHyperarcVector);

			for(int i=0;i<feasileHyperarcVector.size();i++)
				res.feasibleHyperarcs.push_back(feasileHyperarcVector[i]);
			for(int i=0;i<feasileNodeVector.size();i++)
				res.feasibleNodes.push_back(feasileNodeVector[i]);
		}

	}
	return true;
}


int main(int argc, char **argv)
{
	// create an empty graph
	string name = "DEFAULT";
	graphVector.emplace_back(make_shared <AOgraph>(name));
	const char* home=getenv("HOME");
	string andor_path(home);
	andor_path=andor_path+"/catkin_ws/src/ANDOR/andor/files/screwing_task.txt";
//	string description = "~/HRI/ANDOR/screwing_task.txt";
	graphVector.back()->loadFromFile(andor_path);


	ros::init(argc, argv, "andor");
	ros::NodeHandle nh;
	ros::ServiceServer service = nh.advertiseService("andorService",updateANDOR);

	cout << "*****************" << endl;
	cout << "AND/OR graph is alive: " << endl;

	ros::spin();
	return 1;

}   

