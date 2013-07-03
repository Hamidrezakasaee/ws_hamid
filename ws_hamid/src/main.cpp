#include "ros/ros.h"
#include "std_msgs/String.h"

#include <sstream>

//Gloabal variable
std::string _name="hamid";
ros::Publisher chatter_pub;

void chatterCallback(const std_msgs::String::ConstPtr& msg_in)
{
  ROS_INFO("%s: I heard: [%s]", _name.c_str(),msg_in->data.c_str());
  
  std_msgs::String  msg_out;
  msg_out.data = "hello world";
  chatter_pub.publish(msg_out);
  ROS_INFO("%s: will publish a message", _name.c_str());
}

int main(int argc, char **argv)
{
  
  ros::init(argc, argv, _name);
  ros::NodeHandle n;
  chatter_pub = n.advertise<std_msgs::String>("player_out", 1);
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