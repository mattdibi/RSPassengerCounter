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
using namespace std::chrono;

// The XCOMPILER uses a different OpenCV version from my main machine
// so I ended up splitting the code using this constant

// Constants
#define MAXRGBVAL 255
#define NODATA 0

#define RED   Scalar(0,0,255)
#define BLUE  Scalar(255,0,0)
#define GREEN Scalar(0,255,0)
#define WHITE Scalar(255,255,255)
#define BLACK Scalar(0,0,0)

// Camera settings
#define IMAGE_WIDTH     640
#define IMAGE_HEIGHT    480
#define FRAMERATE       60

// Calibration starting values
#define MAX_RANGE_METER 43
#define BLUR_KSIZE 10
#define AREA_MIN 10000     // This depends on the camera distance from the passengers
#define X_NEAR 40
#define Y_NEAR 90
#define MAX_PASSENGER_AGE 60 // 60 FPS * 1 seconds (HP: 60fps camera)

#define MAX_1PASS_AREA 60000
#define MAX_2PASS_AREA 90000

void displayHelp()
{
    cout << "HELP Avalable modes:\n";
    cout << "              - Without arguments: it opens the default webcam and captures the input stream.\n";
    cout << "-s <filename> - Capture mode: it saves all the stream on file.\n";
    cout << "-c            - Calibration mode: it opens the default webcam and display calibration trackbars.\n";
    cout << "-h            - Display help.\n";
    return;
}

int main(int argc, char * argv[])
{
    bool calibrationOn = false;

    // Calibration
    int thresholdCentimeters = MAX_RANGE_METER;
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

    // Execution time
    duration<double> loopTime;
    bool firstLoop = true;

    // Saving videos
    bool saveVideo = false;
    string fileName;

    VideoWriter outputVideoColor;
    VideoWriter outputVideoDepth;
    VideoWriter outputVideoFrame;

    // --INITIALIZE VIDEOCAPTURE
    if(argc >= 2)
    {
        if(!strcmp(argv[1], "-c"))
            calibrationOn = true;
        else if(!strcmp(argv[1], "-s"))
        {
            saveVideo = true;
            fileName = argv[2];

            Size S(IMAGE_WIDTH,IMAGE_HEIGHT);

            // IMPORTANT: 30 FPS was found after experimenting
            outputVideoColor.open(fileName + "-color.avi", CV_FOURCC('M','J','P','G'), 30, S);
            outputVideoDepth.open(fileName + "-depth.avi", CV_FOURCC('M','J','P','G'), 30, S);
            //outputVideoFrame.open(fileName + "-frame.avi", CV_FOURCC('M','J','P','G'), 30, S);

            if (!outputVideoColor.isOpened())
            {
                cout  << "Could not open the output video for write\n ";
                return -1;
            }

        }
        else if(!strcmp(argv[1], "-h"))
        {
            // --HELP
            displayHelp();
            return 0;
        }
        else
        {
            // --HELP
            displayHelp();
            return 0;
        }
    }

    // Get first realsense device
    context ctx;
    device * dev = ctx.get_device(0);

    // Display device INFORMATION
    cout << "Device: " << dev->get_name() << "\n";
    cout << "Serial number: " << dev->get_serial() << "\n";
    cout << "Firmware version: " << dev->get_firmware_version() << "\n";

    // R200 camera settings
    string devName = dev->get_name();
    if(devName.compare("Intel RealSense R200") == 0)
    {
        // apply_depth_control_preset(dev, 5);
        // dev->set_option(rs::option::r200_lr_auto_exposure_enabled, 0);
        // ...
    }

    // Global camera settings
    //dev->set_option(rs::option::color_exposure, 500);

    // Configure stream
    dev->enable_stream(rs::stream::color, IMAGE_WIDTH, IMAGE_HEIGHT, rs::format::bgr8, FRAMERATE);
    dev->enable_stream(rs::stream::depth, IMAGE_WIDTH, IMAGE_HEIGHT, rs::format::z16, FRAMERATE);

    // Start streaming
    dev->start();

    // --SETUP WINDOWS
    if(!saveVideo)
    {
        namedWindow("Frame",WINDOW_AUTOSIZE);
        namedWindow("Color",WINDOW_AUTOSIZE);
    }

    // --GRAB AND WRITE LOOP
    cout << "Start grabbing loop\n";

    // Framerate calculation variables
    int frames = 0; 
    float time = 0, fps = 0;
    auto tf0 = std::chrono::high_resolution_clock::now();

    while(1)
    {
        //-- PERFORMANCE ESTMATION
        high_resolution_clock::time_point t1 = high_resolution_clock::now(); //START

        // Synchronization
        if( dev->is_streaming( ) )
        {
            //dev->wait_for_frames( );
            dev->poll_for_frames(); // Non blocking option
        }

        // Framerate
        auto tf1 = std::chrono::high_resolution_clock::now();
        time += std::chrono::duration<float>(tf1-tf0).count();
        tf0 = tf1;
        ++frames;
        if(time > 0.5f)
        {
            fps = frames / time;
            frames = 0;
            time = 0;
        }

        // Get frame data
        Mat color(Size(IMAGE_WIDTH, IMAGE_HEIGHT), CV_8UC3, (void*)dev->get_frame_data(rs::stream::color), Mat::AUTO_STEP);
        Mat depth(Size(IMAGE_WIDTH, IMAGE_HEIGHT), CV_16U , (void*)dev->get_frame_data(rs::stream::depth_aligned_to_color), Mat::AUTO_STEP);

        /* ********************** EXPERIMENT *************************** */
        Mat tmp = depth.clone(); //Deep copy (tmp has its own copy of the pixels of depth)

        // Finding current scale
        float scale = dev->get_depth_scale();

        double min;
        double max;
        Point tmpMinLoc;
        Point tmpMaxLoc;

        int nearestVal;
        Point nearestLoc;

        int farthestVal;
        Point farthestLoc;

        /* ****************************************************************************************************************
        NOTE:
        The depth matrix has a strange standard:
             0 = No depth data
             1 - 65535 = Depth where 1 is the nearest and 65535 is the farthest
        I want to have 65535 as the new No depth data values
        
        Furthermore, while displaying the depth data, I want to have the farthest object in black and the nearest in white
        this took some mental gymnastic to be implemented
        **************************************************************************************************************** */

        // Saving farthest point
        minMaxLoc(tmp, &min, &max, &tmpMinLoc, &tmpMaxLoc);
        farthestVal = max;
        farthestLoc = tmpMaxLoc;

        // If pixelValue == 0 set it to 65535( = 2^16 - 1)
        tmp.setTo(65535, tmp == NODATA);

        // Saving nearest point
        minMaxLoc(tmp, &min, &max, &tmpMinLoc, &tmpMaxLoc);
        nearestVal = min;
        nearestLoc = tmpMinLoc;

        // Converts CV_16U to CV_8U using a scale factor of 255.0/ 65535
        tmp.convertTo(tmp, CV_8UC1, 255.0 / 65535);

        // Current situation: Nearest object => Black, Farthest object => White
        // We want to have  : Nearest object => White, Farthest object => Black
        tmp =  cv::Scalar::all(255) - tmp;

        // Color map: Nearest object => Red, Farthest object => Blue
        equalizeHist( tmp, tmp );
        applyColorMap(tmp, tmp, COLORMAP_JET);

        // Highlight nearest and farthest pixel
        circle( tmp, nearestLoc, 5, WHITE, 2, 8, 0 );
        putText(tmp, "Nearest: " + to_string(nearestVal*scale) + " m", Point(0,tmp.rows - 10), FONT_HERSHEY_SIMPLEX, 0.5, WHITE, 2);

        circle( tmp, farthestLoc, 5, WHITE, 2, 8, 0 );
        putText(tmp, "Farthest: " + to_string(farthestVal*scale) + " m", Point(color.cols - 310, color.rows - 10), FONT_HERSHEY_SIMPLEX, 0.5, WHITE, 2);

        // Display result
        if(!saveVideo)
        {
            namedWindow("Distance", WINDOW_AUTOSIZE);
            imshow("Distance",tmp);
        }
        /* ********************** EXPERIMENT *************************** */

        
        
        // OLDVER: Without thresholding
        // frame = conversion(depth);
        // depth.convertTo( frame, CV_8UC1);
        
        // NEWVER: With variable threshold
        depth.setTo(65535, depth == NODATA);
        depth.convertTo(depth, CV_32FC1); // Threshold only accepts CV_8U or CV_32F types

        int threshPixel = thresholdCentimeters / (100*scale);
        threshold(depth, depth, threshPixel, 65535, THRESH_BINARY);

        depth.convertTo(depth, CV_8UC1, 255.0 / 65535); // Convert to CV_8U

        depth =  cv::Scalar::all(255) - depth;

        frame = depth.clone();

        // Horizontal line     
        line( color,
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
        for(unsigned int idx = 0; idx < hierarchy.size(); idx++)
        {
            // Draw contours for every detected object
            // drawContours( color, contours, idx, Scalar(0,255,0), 2, 8, hierarchy, 0, Point(0,0) );

            // -- AREA
            // Calculating area
            double areaCurrentObject = contourArea(contours[idx]);

            // If calculated area is big enough begin tracking the object
            if(areaCurrentObject > areaMin)
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

                // Debugging multiple passenger count + calibration
                // if(areaCurrentObject > MAX_1PASS_AREA && areaCurrentObject < MAX_2PASS_AREA)
                //     putText(color, "Area: " + to_string(areaCurrentObject) + " = 2 PASSENGERS", mc, FONT_HERSHEY_SIMPLEX, 0.5, RED, 2);
                // else if(areaCurrentObject > MAX_2PASS_AREA)
                //     putText(color, "Area: " + to_string(areaCurrentObject) + " = 3 PASSENGERS", mc, FONT_HERSHEY_SIMPLEX, 0.5, RED, 2);
                // else
                //     putText(color, "Area: " + to_string(areaCurrentObject) + " = 1 PASSENGERS", mc, FONT_HERSHEY_SIMPLEX, 0.5, RED, 2);

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
                        if(passengers[i].getTracks().size() > 1)
                        {
                            // Up to down
                            if( (passengers[i].getLastPoint().y < frame.rows/2 && passengers[i].getCurrentPoint().y >= frame.rows/2) ||
                                (passengers[i].getLastPoint().y <= frame.rows/2 && passengers[i].getCurrentPoint().y > frame.rows/2) )
                            {
                                // Counting multiple passenger depending on area size
                                if (areaCurrentObject > MAX_1PASS_AREA && areaCurrentObject < MAX_2PASS_AREA)
                                    cnt_out += 2;
                                else if (areaCurrentObject > MAX_2PASS_AREA)
                                    cnt_out += 3;
                                else
                                    cnt_out++;

                                // Logging count
                                cout << "ID: " << passengers[i].getPid() << " crossed going U to D.\n";

                                // Visual feedback
                                circle(color, Point(color.cols - 20, 20), 8, RED, CV_FILLED);
                            }

                            // Down to up
                            if( (passengers[i].getLastPoint().y > frame.rows/2 && passengers[i].getCurrentPoint().y <= frame.rows/2) ||
                                (passengers[i].getLastPoint().y >= frame.rows/2 && passengers[i].getCurrentPoint().y < frame.rows/2) )
                            {
                                // Counting multiple passenger depending on area size
                                if (areaCurrentObject > MAX_1PASS_AREA && areaCurrentObject < MAX_2PASS_AREA)
                                    cnt_in += 2;
                                else if (areaCurrentObject > MAX_2PASS_AREA)
                                    cnt_in += 3;
                                else
                                    cnt_in++;

                                // Logging count
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
            if(passengers[i].getTracks().size() > 0)
            {
                polylines(color, passengers[i].getTracks(), false, passengers[i].getTrackColor(),2);
                //putText(color, "Pid: " + to_string(passengers[i].getPid()), passengers[i].getCenter(), FONT_HERSHEY_SIMPLEX, 0.5, passengers[i].getTrackColor(), 2);
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

        // Debugging
        // putText(color, "Tracked passengers: " + to_string(passengers.size()), Point(15,  15) , FONT_HERSHEY_SIMPLEX, 0.5, RED, 2);
        putText(color, "FPS: " + to_string(fps), Point(15,  15) , FONT_HERSHEY_SIMPLEX, 0.5, RED, 2);

        // --PRINTING INFORMATION
        putText(color, "Count IN: " + to_string(cnt_in), Point(0,color.rows - 10) , FONT_HERSHEY_SIMPLEX, 1, WHITE, 2);
        putText(color, "Count OUT: " + to_string(cnt_out), Point(color.cols - 310, color.rows - 10) , FONT_HERSHEY_SIMPLEX, 1, WHITE, 2);

        // --CALIBRATION TRACKBARS
        if(calibrationOn)
        {
            createTrackbar("Threshold", "Color", &thresholdCentimeters, 400);
            createTrackbar("Blur", "Color", &blur_ksize, 100);
            createTrackbar("xNear", "Color", &xNear, IMAGE_WIDTH);
            createTrackbar("yNear", "Color", &yNear, IMAGE_HEIGHT);
            createTrackbar("Area min", "Color", &areaMin, 100000);
            createTrackbar("Passenger age", "Color", &maxPassengerAge, 300);
        }

        // Show videos
        if(!saveVideo)
        {
            imshow("Frame",frame);
            imshow("Color", color);
        }

        // -- SAVING VIDEOS
        if(saveVideo)
        {
            //outputVideoFrame.write(frame);
            outputVideoDepth.write(tmp);
            outputVideoColor.write(color);
        }

        // --PERFORMANCE ESTMATION
        high_resolution_clock::time_point t2 = high_resolution_clock::now(); //STOP

        if(firstLoop)
            loopTime = duration_cast<duration<double>>(t2 - t1);
        else
        {
            loopTime += duration_cast<duration<double>>(t2 - t1);
            loopTime = loopTime/2;
        }

        waitKey(1);

        firstLoop = false;

    }

    cout << "Execution time:\n";
    cout << "Loop: " << loopTime.count() << " seconds\n";

    destroyAllWindows(); 
    return 0;
}

