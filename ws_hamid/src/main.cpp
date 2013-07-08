#include "ros/ros.h"
#include "std_msgs/String.h"
#include <sstream>
#include <ws_referee/custom.h>
#include <ws_referee/randomize.h>
#include <visualization_msgs/Marker.h>
#include <tf/transform_broadcaster.h>
#include <tf/transform_listener.h>
#include <turtlesim/Pose.h>
#include <ws_referee/MovePlayerTo.h>



//Gloabal variable
std::string _name="hamid";
std::string _police_player="hamid";
ros::Publisher chatter_pub;
ros::Publisher marker_pub;
ros::ServiceClient* client;
ros::NodeHandle *n;


double _pos_x;
double _pos_y;

tf::TransformBroadcaster *br;
tf::Transform transform;
tf::TransformListener *listener;

//void chatterCallback(const std_msgs::String::ConstPtr& msg_in)



void chatterCallback(const ws_referee::custom::ConstPtr& msg_in)
{
  //ROS_INFO("%s: I heard: [%s]", _name.c_str(),msg_in->data.c_str());
  //std_msgs::String  msg_out;
  //msg_out.data = "hello world";
  //chatter_pub.publish(msg_out);

  ROS_INFO("%s: Received msg wit dist= %f", _name.c_str(), msg_in->dist);
  
  
  // check fo the palayer in the field 
   tf::StampedTransform tf_2;
   bool be_a_police= true; 
   

   
    try
    {
        listener->lookupTransform("world", "tf_"+_police_player, ros::Time(0), tf_2);
    }
    catch(tf::TransformException ex)
    {
        ROS_ERROR("%s", ex.what());
        be_a_police = false;
    }
    

   if (be_a_police)
    { 
      if (!is_in_field (tf_2.getOrigin().x(),tf_2.getOrigin().y() ))
       {
         ROS_INFO("%s: I found that %s is out of the area will send him to the start point", _name.c_str(), _police_player.c_str());
         ws_referee::MovePlayerTo srv;
         srv.request.new_pos_x = -5;
         srv.request.new_pos_y = 0;
         srv.request.player_that_requested=_name;  
         if (client->call(srv))
         {}
       else
          { 
            ROS_ERROR ("failed to call service ... ");
       
          } 
       } 
    }


  tf::Transform tf_tmp;
  tf_tmp.setOrigin( tf::Vector3(msg_in->dist, 0.0, 0.0) );
  tf_tmp.setRotation( tf::Quaternion(0,0,get_random_deg()*M_PI/180,1));
  br->sendTransform(tf::StampedTransform(tf_tmp, ros::Time::now(), "tf_"+_name, "tf_tmp"+_name));	  
  
    // query tranform world to tf_tmp_vahid
    tf::StampedTransform tf_1;
    try{
        listener->lookupTransform("world", "tf_tmp" + _name, ros::Time(0), tf_1);
    }
    catch(tf::TransformException ex){
        ROS_ERROR("%s", ex.what());
    }
    
    
  //send transform 
  transform= tf_1;
   br->sendTransform(tf::StampedTransform(transform, ros::Time::now(), "world", "tf_"+_name));
  
    
  //_pos_x+=msg_in->dist;
   bool should_quit=false;


  //Publish new msg
  ws_referee::custom msg_out;
  msg_out.sender = _name;
  msg_out.dist=get_random_num();
  
  if (msg_out.winner!="")
    {
     ROS_INFO("\n\n ------------ \n\n");
     msg_out.winner = msg_in->winner;
     msg_out.dist = 0;
     should_quit=true;
     }

 // else if (_pos_x>5)
 //  {
  //  ROS_INFO("\n\n I WON - HAMID \n\n");
    // msg_out.winner="Hamid";
     //msg_out.dist = 0;
     //should_quit=true;
//   }

 else
   {
      msg_out.winner = "";
      msg_out.dist = get_random_num(); 
   }

  chatter_pub.publish(msg_out);
  ROS_INFO("%s: I'm at %f. will publish a message", _name.c_str(), _pos_x);


//marker value 
	visualization_msgs::Marker marker;
	marker.header.frame_id = "tf_"+_name;
	marker.header.stamp = ros::Time();
	marker.ns = "";
	marker.id = 0;
	marker.type = visualization_msgs::Marker::SPHERE;
	marker.action = visualization_msgs::Marker::ADD;
	marker.pose.position.x = 0;
	marker.pose.position.y = 0;
	marker.pose.position.z = 0;
	marker.pose.orientation.x = 0.0;
	marker.pose.orientation.y = 0.0;
	marker.pose.orientation.z = 0.0;
	marker.pose.orientation.w = 1.0;
	marker.scale.x = 0.3;
	marker.scale.y = 0.3;
	marker.scale.z = 0.3;
	marker.color.a = 1.0;
	marker.color.r = 1.0;
	marker.color.g = 0.0;
	marker.color.b = 0.0;
	marker_pub.publish( marker );

    // a new text marker
    marker.id = 1;
    marker.color.r = 0.5;
    marker.pose.position.x = - 0.3;
    marker.type = visualization_msgs::Marker::TEXT_VIEW_FACING;
    marker.text = "hamid";
    marker_pub.publish( marker );


// transformation 

  //transform.setOrigin( tf::Vector3(_pos_x, _pos_y, 0.0) );
  //transform.setRotation( tf::Quaternion(0,0,0,1));
  br->sendTransform(tf::StampedTransform(transform, ros::Time::now(), "world", "tf_"+_name));
ros::Duration(0.5).sleep();

if (should_quit==true)
	{
		ROS_INFO("%s: I will shutdown",_name.c_str());
		ros::shutdown();
	}

}

bool serviceCallback(ws_referee::MovePlayerTo::Request &req,ws_referee::MovePlayerTo::Response &res)
{
   ROS_INFO("%s: Damn %s send to me X= %f, Y=%f", _name.c_str(), req.player_that_requested.c_str(), req.new_pos_x, req.new_pos_y);
   // send the transformation 
   transform.setOrigin( tf::Vector3(req.new_pos_x, req.new_pos_y, 0.0) );
   transform.setRotation( tf::Quaternion(0, 0, 0, 1) );
   br->sendTransform(tf::StampedTransform(transform, ros::Time::now(), "world", "tf_" + _name ));
   
   res.reply ="I will have my revenge!";
   return true;
}



int main(int argc, char **argv)
{
  ros::init(argc, argv, _name);
  n=(ros::NodeHandle*)new(ros::NodeHandle);
  
  init_randomization_seed();
  
  chatter_pub = n->advertise<ws_referee::custom>("player_out", 1);
  marker_pub = n->advertise<visualization_msgs::Marker>("hamid_marker", 1);

  br =(tf::TransformBroadcaster*)new (tf::TransformBroadcaster);
  listener= (tf::TransformListener*) new (tf::TransformListener);


  
  
   init_randomization_seed();
  _pos_x=-5;
  _pos_y=-4;

//set teransformation 
  ros::Time t = ros::Time::now();
  transform.setOrigin( tf::Vector3(_pos_x, _pos_y, 0.0) );
  transform.setRotation( tf::Quaternion(0,0,0,1));
  br->sendTransform(tf::StampedTransform(transform, t, "world", "tf_"+_name));

  tf::Transform tf_tmp;
  tf_tmp.setOrigin( tf::Vector3(0, 0.0, 0.0) );
  tf_tmp.setRotation( tf::Quaternion(0,0,0,1)); 
  br->sendTransform(tf::StampedTransform(tf_tmp, t, "tf_" + _name, "tf_tmp" + _name));

  ros::spinOnce();
  ros::Duration(0.3).sleep();

  //at time now
  transform.setOrigin( tf::Vector3(_pos_x, _pos_y, 0.0) );
  transform.setRotation( tf::Quaternion(0, 0, 0, 1) );
  br->sendTransform(tf::StampedTransform(transform, ros::Time::now(), "world", "tf_" + _name ));

  tf_tmp.setOrigin( tf::Vector3(0, 0.0, 0.0) );
  tf_tmp.setRotation( tf::Quaternion(0, 0, 0, 1) );
  br->sendTransform(tf::StampedTransform(tf_tmp, ros::Time::now(), "tf_" + _name, "tf_tmp" + _name ));
  
  client= (ros::ServiceClient*) new (ros::ServiceClient);
  *client = n->serviceClient<ws_referee::MovePlayerTo>("move_player_"+_police_player);
  
	
  ros::ServiceServer service = n->advertiseService("move_player_" + _police_player, serviceCallback);
  ros::Subscriber sub = n->subscribe("player_in", 1, chatterCallback);
  ros::Rate loop_rate(2);

  ROS_INFO("%s: Hamid Node Started", _name.c_str());

  int count = 0;
  ros::MultiThreadedSpinner spinner(4); // Use 4 threads
  spinner.spin(); // spin() will not return until the node has been shutdown

  
  //while (ros::ok())
  //{
    //ros::spinOnce();
    //loop_rate.sleep();
    //++count;
  //}


  return 0;
}
