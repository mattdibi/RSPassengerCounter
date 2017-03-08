/**
    PCN (Passenger CouNter)
    Purpose: Creating a computer vision system that counts passengers entering/exiting a door in a transportation environment

    @author Mattia Dal Ben
    @version 1.0 23/02/2017

*/

#include <iostream>
#include <string>
#include <chrono>

#include <librealsense/rs.hpp>
#include <opencv2/opencv.hpp>

#include "passenger.cpp"

using namespace cv;
using namespace std;
using namespace rs;

// The XCOMPILER uses a different OpenCV version from my main machine
// so I ended up splitting the code using this constant

// Constants
#define MAXRGBVAL 255

#define RED   Scalar(0,0,255)
#define BLUE  Scalar(255,0,0)
#define GREEN Scalar(0,255,0)
#define WHITE Scalar(255,255,255)

// Calibration starting values
#define BLUR_KSIZE 10
#define AREA_MIN 6000     // This depends on the camera distance from the passengers
#define X_NEAR 100
#define Y_NEAR 100
#define MAX_PASSENGER_AGE 60 // 60 FPS * 1 seconds (HP: 60fps camera)

void displayHelp()
{
    cout << "HELP Avalable modes:\n";
    cout << "              - Without arguments: it opens the default webcam and captures the input stream.\n";
    cout << "-c            - Calibration mode: it opens the default webcam and display calibration trackbars.\n";
    cout << "-h            - Display help.\n";
    return;
}

int main(int argc, char * argv[])
{
    bool calibrationOn = false;

    // Calibration
    int blur_ksize = BLUR_KSIZE;
    int areaMin = AREA_MIN;
    int xNear = X_NEAR;
    int yNear = Y_NEAR;
    int maxPassengerAge = MAX_PASSENGER_AGE;

    // Passenger counters
    int cnt_in  = 0;
    int cnt_out = 0;

    // Passengers tracker
    int pid = 0;
    vector<Passenger> passengers;

    // Streams
    Mat frame;
    Mat morphTrans;

    // Contours variables
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    // --INITIALIZE VIDEOCAPTURE
    if(argc >= 2)
    {
        if(!strcmp(argv[1], "-c"))
            calibrationOn = true;
        else if(!strcmp(argv[1], "-h"))
        {
            // --HELP
            displayHelp();
            return 0;
        }
    }
    else
    {
        // --HELP
        displayHelp();
        return 0;
    }

    // Get first realsense device
    context ctx;
    device * dev = ctx.get_device(0);

    // Configure Infrared stream to run at VGA resolution at 30 frames per second
    dev->enable_stream(rs::stream::color, 640, 480, rs::format::bgr8, 60);
    dev->enable_stream(rs::stream::depth, 640, 480, rs::format::z16, 60);

    // Start streaming
    dev->start();

    // --SETUP WINDOWS
    namedWindow("Frame",WINDOW_AUTOSIZE);
    namedWindow("Color",WINDOW_AUTOSIZE);

    // --GRAB AND WRITE LOOP
    cout << "Start grabbing loop\n";

    while(1)
    {
        // Synchronization
        if( dev->is_streaming( ) )
            dev->wait_for_frames( );

        // Get frame data
        Mat color(Size(640, 480), CV_8UC3, (void*)dev->get_frame_data(rs::stream::color), Mat::AUTO_STEP);
        Mat depth(Size(640, 480), CV_16U , (void*)dev->get_frame_data(rs::stream::depth), Mat::AUTO_STEP);

        // frame = conversion(depth);
        depth.convertTo( frame, CV_8UC1, 255.0/1000 );
        
        // Horizontal line     
        line( color,
              // Vertical line
              //Point(frame.cols/2,0),            //Starting point of the line
              //Point(frame.cols/2,frame.rows),   //Ending point of the line  
              // Horizontal line
              Point(0,color.rows/2),            //Starting point of the line
              Point(color.cols,color.rows/2),   //Ending point of the line
              RED,                              //Color
              2,                                //Thickness
              8);                               //Linetype

        // Blurring the image
        blur(frame, morphTrans, Size(blur_ksize,blur_ksize));

        // --FINDING CONTOURS
        findContours(morphTrans, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);

        // For every detected object
        for(int idx = 0 ; idx >= 0; idx = hierarchy[idx][0] )
        {
            // Draw contours for every detected object
            // drawContours( color, contours, idx, Scalar(0,255,0), 2, 8, hierarchy, 0, Point(0,0) );

            // -- AREA
            // Calculating area
            double area1 = contourArea(contours[idx]);

            // If calculated area is big enough begin tracking the object
            if(area1 > areaMin)
            {
                // --TRACKING
                // Getting mass center
                Moments M = moments(contours[idx]);
                Point2f mc = Point2f( M.m10/M.m00 , M.m01/M.m00 );

                // Getting bounding rectangle
                Rect br = boundingRect(contours[idx]);

                // Drawing mass center and bounding rectangle
                rectangle( color, br.tl(), br.br(), GREEN, 2, 8, 0 );
                circle( color, mc, 5, RED, 2, 8, 0 );

                // --PASSENGERS DB UPDATE
                bool newPassenger = true;
                for(unsigned int i = 0; i < passengers.size(); i++)
                {
                    // If the passenger is near a known passenger assume they are the same one
                    if( abs(mc.x - passengers[i].getX()) <= xNear &&
                        abs(mc.y - passengers[i].getY()) <= yNear )
                    {
                        // Update coordinates
                        newPassenger = false;
                        passengers[i].updateCoords(mc);

                        // --COUNTER
                        if(passengers[i].getTracks().size() > 2)
                        {
                            // Left to right
                            // if(passengers[i].getLastPoint().x < frame.cols/2 &&
                            //    passengers[i].getCurrentPoint().x > frame.cols/2)

                            // Up to down
                            if( (passengers[i].getLastPoint().y < frame.rows/2 && passengers[i].getCurrentPoint().y >= frame.rows/2) ||
                                (passengers[i].getLastPoint().y <= frame.rows/2 && passengers[i].getCurrentPoint().y > frame.rows/2) )
                            {
                                cnt_out++;

                                cout << "ID: " << passengers[i].getPid() << " crossed going U to D.\n";

                                // Visual feedback
                                circle(color, Point(color.cols - 20, 20), 8, RED, CV_FILLED);
                            }

                            // Right to left
                            // if(passengers[i].getLastPoint().x > frame.cols/2 &&
                            //    passengers[i].getCurrentPoint().x < frame.cols/2)

                            // Down to up
                            if( (passengers[i].getLastPoint().y > frame.rows/2 && passengers[i].getCurrentPoint().y <= frame.rows/2) ||
                                (passengers[i].getLastPoint().y >= frame.rows/2 && passengers[i].getCurrentPoint().y < frame.rows/2) )
                            {
                                cnt_in++;

                                cout << "ID: " << passengers[i].getPid() << " crossed going D to U.\n";

                                // Visual feedback
                                circle(color, Point(color.cols - 20, 20), 8, GREEN, CV_FILLED);
                            }

                        }

                        break;
                    }
                }

                // If wasn't near any known object is a new passenger
                if(newPassenger)
                {
                    Passenger p(pid, mc);
                    passengers.push_back(p);
                    pid++;
                }
            }
        }

        // For every passenger in passengers DB
        for(unsigned int i = 0; i < passengers.size(); i++)
        {
            // -- DRAWING PASSENGER TRAJECTORIES
            if(passengers[i].getTracks().size() > 2)
            {
                polylines(color, passengers[i].getTracks(), false, passengers[i].getTrackColor(),2);
                //putText(frame, "Pid: " + to_string(passengers[i].getPid()), passengers[i].getCenter(), FONT_HERSHEY_SIMPLEX, 0.5, passengers[i].getTrackColor(), 2);
            }

            // --UPDATE PASSENGER STATS
            // Updating age
            passengers[i].updateAge();

            // Removing older passengers
            // NB: The age depends on the FPS that the camera is capturing!
            if(passengers[i].getAge() > maxPassengerAge)
            {
                passengers.erase(passengers.begin() +i);
            }
        }

        // --PRINTING INFORMATION
        putText(color, "Count IN: " + to_string(cnt_in), Point(0,color.rows - 10) , FONT_HERSHEY_SIMPLEX, 1, WHITE, 2);
        putText(color, "Count OUT: " + to_string(cnt_out), Point(color.cols - 310, color.rows - 10) , FONT_HERSHEY_SIMPLEX, 1, WHITE, 2);

        // --CALIBRATION TRACKBARS
        if(calibrationOn)
        {
            createTrackbar("Blur", "Color", &blur_ksize, 100);
            createTrackbar("xNear", "Color", &xNear, 250);
            createTrackbar("yNear", "Color", &yNear, 250);
            createTrackbar("Area min", "Color", &areaMin, 10000);
            createTrackbar("Passenger age", "Color", &maxPassengerAge, 300);
        }

        // Show videos
        imshow("Frame",frame);
        imshow("Color", color);

        if(waitKey(30) == 27) //wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed by user" << endl; 
            break;
        }

    }

    destroyAllWindows(); 
    return 0;
}

