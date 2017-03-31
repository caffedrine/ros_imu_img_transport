#include <ros/ros.h>

//OpenCv and the company
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>

//Mesajul nostru personalizat
#include "ros_imu_img_transport/mesaj.h"
#include "sensor_msgs/Imu.h"

//C++ Libs
#include <iostream>
#include <sstream>

//For random numbers
#include <cstdlib>
#include <ctime>

ros_imu_img_transport::mesaj initMSG()
{
    ros_imu_img_transport::mesaj msg;

    //Init image from msg
    msg.image.header.seq = 0;
    msg.image.header.frame_id = "0";
    msg.image.header.stamp = ros::TIME_MIN;

    msg.image.height = 0;
    msg.image.width = 0;

    //Init imu fields
    msg.imu.isValidIMU = 0;
    msg.imu.imu.angular_velocity.x = 0;
    msg.imu.imu.angular_velocity.y = 0;
    msg.imu.imu.angular_velocity.z = 0;

    msg.imu.imu.linear_acceleration.x = 0;
    msg.imu.imu.linear_acceleration.y = 0;
    msg.imu.imu.linear_acceleration.z = 0;

    msg.imu.imu.orientation.w = 0;
    msg.imu.imu.orientation.x = 0;
    msg.imu.imu.orientation.y = 0;
    msg.imu.imu.orientation.z = 0;

    return msg;
}

sensor_msgs::Imu updateIMU()
{
    //Sergey, this is for you! Fetch your IMU vals and fill fields.

    sensor_msgs::Imu imu;

    //Fetch IMU data here
    imu.angular_velocity.x = 0;
    imu.angular_velocity.y = 0;
    imu.angular_velocity.z = 0;

    imu.linear_acceleration.x = 0;
    imu.linear_acceleration.y = 0;
    imu.linear_acceleration.z = 0;

    imu.orientation.w = 0;
    imu.orientation.x = 0;
    imu.orientation.y = 0;
    imu.orientation.z = 0;

    //Overwrite random values for debug
    imu.angular_velocity.x = (1 + rand() % 200);
    imu.angular_velocity.y = (1 + rand() % 200);
    imu.angular_velocity.z = (1 + rand() % 200);

    return imu;
}

int main(int argc, char** argv)
{
    //Used to generate random data for IMU tests.
    std::srand ( time(NULL) );

    ros::init(argc, argv, "image_publisher");
    ros::NodeHandle n;

    ros::Publisher publisher = n.advertise<ros_imu_img_transport::mesaj>("camera_and_imu", 1) ;

    //Video editing and all the stuff
    // Convert the passed as command line parameter index for the video device to an integer
    std::istringstream video_sourceCmd("0");
    int video_source;

    if(!(video_sourceCmd >> video_source)) return 1;

    cv::VideoCapture cap(video_source);

    // Check if video device can be opened with the given index
    if(!cap.isOpened()) return 1;
    cv::Mat frame;

    //Define essage variable we will publish
    ros_imu_img_transport::mesaj msg = initMSG(); //fill all fields with 0 values
    unsigned int seq = 0;  //this is sequence transmission ID;

    ros::Rate loop_rate(1);  //Vrei mai multe cadre pe secunda? Modifica asta :)
    while (n.ok())
    {
        //Increment transmission ID
        seq++;

        //Creating header of image. This will contains all info!
        std_msgs::Header header;
        header.seq = seq;
        std::stringstream ss; ss << "frstr" << seq;
        header.frame_id = ss.str();
        header.stamp = ros::Time::now();

        //Fill some random IMU data
        msg.imu.isValidIMU = 0;     //Values are not valid
        msg.imu.imu = updateIMU();  //Filling IMU fields with values

        //Get a new frame from camera
        cap >> frame;

        // Check if grabbed frame is actually full with some content
        if(!frame.empty())
        {
            cv_bridge::CvImage img_bridge = cv_bridge::CvImage(header, sensor_msgs::image_encodings::BGR8, frame);
            img_bridge.toImageMsg(msg.image);

            //Display transmission sequence ID
            std::cout << "[SEQ: " << msg.image.header.seq << "]";

            //Display frame_id - I associate frame_id with seq_id. If changed, it will be forwarded
            std::cout << "[F_ID: " << msg.image.header.frame_id << "]";

            //Display some IMU data vefore sending
            std::cout << "[IMU X:" << msg.imu.imu.angular_velocity.x << " ";
            std::cout << "Y: " << msg.imu.imu.angular_velocity.y << " ";
            std::cout << "Z: " << msg.imu.imu.angular_velocity.z << "]" << std::endl;

            publisher.publish(msg);
            cv::waitKey(1);
        }

        ros::spinOnce();
        loop_rate.sleep();
    }
}
