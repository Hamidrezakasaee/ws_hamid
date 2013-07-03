#include "ros/ros.h"
#include "std_msgs/String.h"
#include <sstream>
#include <ws_referee/custom.h>

//Gloabal variable
std::string _name="hamid";
ros::Publisher chatter_pub;
double _pos_x;
double _pos_y;

//void chatterCallback(const std_msgs::String::ConstPtr& msg_in)
void chatterCallback(const ws_referee::custom::ConstPtr& msg_in)
{
  //ROS_INFO("%s: I heard: [%s]", _name.c_str(),msg_in->data.c_str());
    ROS_INFO("%s: I heard: [%s]", _name.c_str());
  //std_msgs::String  msg_out;
  //msg_out.data = "hello world";
  //chatter_pub.publish(msg_out);

  ROS_INFO("%s: Received msg wit dist= %f", _name.c_str(), msg_in->dist);
  _pos_x+=msg_in->dist;

  ws_referee::custom  msg_out;
  msg_out.dist=0.5;
  msg_out.sender =_name;
  msg_out.winner= "";
  chatter_pub.publish(msg_out);

  ROS_INFO("%s: I'm at %f. will publish a message", _name.c_str(), _pos_x);
}

int main(int argc, char **argv)
{
  

  ros::init(argc, argv, _name);
  ros::NodeHandle n;
  chatter_pub = n.advertise<ws_referee::custom>("player_out", 1);
  _pos_x=0;
  _pos_y=3;

  ros::Subscriber sub = n.subscribe("player_in", 1, chatterCallback);
  ros::Rate loop_rate(2);
  ROS_INFO("%s: Hamid Node Started", _name.c_str());

  int count = 0;
  while (ros::ok())
  {
    ros::spinOnce();
    loop_rate.sleep();
    ++count;
  }


  return 0;
}
