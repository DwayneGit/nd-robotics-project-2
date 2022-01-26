#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

ros::ServiceClient client;

void drive_robot(float lin_x, float ang_z) {

    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    if (!client.call(srv))
        ROS_ERROR("Failed to call service command_robot");	

}

void process_image_callback(const sensor_msgs::Image img) {

	int white_pixel = 255;
	float sect_step = (img.step)/3;
	bool driving = false;

    for (int i = 0; i < img.height * img.step; i++) {
        if (img.data[i] == white_pixel) {
   			float curr_step = i % img.step;
            if(curr_step>=0 && curr_step<sect_step)
				drive_robot(0.0, 0.2);
			else if (curr_step>=sect_step && curr_step<sect_step*2)
				drive_robot(0.2, 0.0);
			else if (curr_step>=sect_step*2)
				drive_robot(0.0, -0.2);
			driving = true;
            break;
        }
    }

	if(!driving)
		drive_robot(0.0, 0.0);
		
}

int main(int argc, char** argv) {

	ros::init(argc, argv, "process_image");
	ros::NodeHandle n;

	client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");
	
	ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

	ros::spin();

	return 0;
}
