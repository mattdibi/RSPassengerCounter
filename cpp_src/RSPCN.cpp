#ifndef RSPCN_CPP
#define RSPCN_CPP

#include "RSPCN.h"

RSPCN::RSPCN(device *assignedDevice) {
    dev = assignedDevice;

    string devName = dev->get_name();

    // Camera settings
    if(devName.compare("Intel RealSense R200") == 0) {
        cameraDevice = R200;

        ImageWidth = IMAGE_WIDTH_R200;
        ImageHeight = IMAGE_HEIGHT_R200;
        CameraFramerate = FRAMERATE_R200;
    }
    else if(devName.compare("Intel RealSense SR300") == 0) {
        cameraDevice = SR300;

        ImageWidth = IMAGE_WIDTH_SR300;
        ImageHeight = IMAGE_HEIGHT_SR300;
        CameraFramerate = FRAMERATE_SR300;
    }

    // Configure stream
    dev->enable_stream(rs::stream::color, ImageWidth, ImageHeight, rs::format::bgr8, CameraFramerate);
    dev->enable_stream(rs::stream::depth, ImageWidth, ImageHeight, rs::format::z16, CameraFramerate);

    // Get device depth scale
    scale = dev->get_depth_scale();
}

void RSPCN::setCameraPresets(int value) {
    //-- CAMERA CONTROL
    if(cameraDevice == R200)
        apply_depth_control_preset(dev, value);
    else {
        apply_ivcam_preset(dev, value);
    }

    return;
}

void RSPCN::toggleCalibration() {
    calibrationOn = !calibrationOn;
    destroyAllWindows();

    return;
}

void RSPCN::toggleDisplayColor() {
    displayColor = !displayColor;
    destroyAllWindows();

    return;
}

void RSPCN::toggleDisplayDepth() {
    displayDepth = !displayDepth;
    destroyAllWindows();

    return;
}

void RSPCN::toggleDisplayRawDepth() {
    displayRawDepth = !displayRawDepth;
    destroyAllWindows();

    return;
}

void RSPCN::toggleDisplayFrame() {
    displayFrame = !displayFrame;
    destroyAllWindows();

    return;
}

void RSPCN::start() {
    thread_ = std::thread(&RSPCN::count, this);
    
    auto myid = thread_.get_id();
    stringstream ss;
    ss << myid;
    threadID = ss.str();
}

void RSPCN::count() {
    // This is needed to avoid threading problems with GTK
    // XInitThreads();

    // Streams
    Mat frame;
    Mat rawDepth;
    Mat morphTrans;
    Mat depthColorMap;

    // Videos
    VideoWriter outputVideoColor;
    VideoWriter outputVideoDepth;
    VideoWriter outputVideoFrame;

    // Contours variables
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    // Calibration
    int thresholdCentimeters = MAX_RANGE_CM;
    int blur_ksize = BLUR_KSIZE;
    int areaMin = AREA_MIN;
    int xNear = X_NEAR;
    int yNear = Y_NEAR;
    int maxPassengerAge = MAX_PASSENGER_AGE;

    // Testing adaptive thershold
    int blockSize = 7;
    int C_100 = 100;

    // Execution time
    duration<double> loopTime;
    bool firstLoop = true;

    // Start streaming
    dev->start();

    // --SETUP WINDOWS
    if(displayColor)
        namedWindow("Color threadID: " + threadID,WINDOW_AUTOSIZE);

    if(displayFrame)
        namedWindow("Frame threadID: " + threadID,WINDOW_AUTOSIZE);

    if(displayRawDepth)
        namedWindow("RawDepth threadID: " + threadID, WINDOW_AUTOSIZE);

    if(displayDepth)
        namedWindow("Distance threadID: " + threadID, WINDOW_AUTOSIZE);

    if(saveVideo) {
        Size S(ImageWidth,ImageHeight);

        outputVideoColor.open((string)dev->get_name() +  threadID + "-color.avi", CV_FOURCC('M','J','P','G'), CameraFramerate, S, true);
        outputVideoDepth.open((string)dev->get_name() +  threadID + "-depth.avi", CV_FOURCC('M','J','P','G'), CameraFramerate, S, true);
        outputVideoFrame.open((string)dev->get_name() +  threadID + "-frame.avi", CV_FOURCC('M','J','P','G'), CameraFramerate, S, true);
    }

    // --GRAB AND WRITE LOOP

    // Framerate calculation variables
    int frames = 0; 
    float time = 0, fps = 0;
    auto tf0 = std::chrono::high_resolution_clock::now();

    while(!halt) {

        //-- PERFORMANCE ESTMATION
        high_resolution_clock::time_point t1 = high_resolution_clock::now(); //START

        // Synchronization
        if( dev->is_streaming( ) ) {
            if(framerateStabilizationOn)
                dev->wait_for_frames( );
            else
                dev->poll_for_frames(); // Non blocking option
        }

        // Framerate
        auto tf1 = std::chrono::high_resolution_clock::now();
        time += std::chrono::duration<float>(tf1-tf0).count();
        tf0 = tf1;
        ++frames;
        if(time > 0.5f) {
            fps = frames / time;
            frames = 0;
            time = 0;
        }

        // Get frame data
        Mat color(Size(ImageWidth, ImageHeight), CV_8UC3, (void*)dev->get_frame_data(rs::stream::color), Mat::AUTO_STEP);
        Mat depth(Size(ImageWidth, ImageHeight), CV_16U , (void*)dev->get_frame_data(rs::stream::depth), Mat::AUTO_STEP);

        // Extracting depth information
        if(displayRawDepth)
            rawDepth = depth.clone();

        if(displayDepth || saveVideo) 
            depthColorMap = getColorMap(depth);
        
        // -- CONVERTING DEPTH IMAGE AND THRESHOLDING
        frame = getFrame(depth, thresholdCentimeters);

        getExperimentalFrame(depth, blockSize, (double)( C_100/ 100 ));

        // -- DENOISING
        blur(frame, morphTrans, Size(blur_ksize,blur_ksize));

        // --FINDING CONTOURS
        findContours(morphTrans, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);

        // For every detected object
        for(unsigned int idx = 0; idx < contours.size(); idx++) {

            // drawContours( color, contours, idx, Scalar(0,255,0), 2, 8, hierarchy, 0, Point(0,0) );

            // -- AREA
            // Calculating area
            double areaCurrentObject = contourArea(contours[idx]);

            // If calculated area is big enough begin tracking the object
            if(areaCurrentObject > areaMin) {
                // --TRACKING
                // Getting mass center
                // Moments M = moments(contours[idx]);
                // Point2f objCenter = Point2f( M.m10/M.m00 , M.m01/M.m00 );

                // Get bounding rectangle
                Rect br = boundingRect(contours[idx]);
                Point2f objCenter = Point2f((int)(br.x + br.width/2) ,(int)(br.y + br.height/2) );

                // Draw mass center and bounding rectangle
                rectangle( color, br.tl(), br.br(), GREEN, 2, 8, 0 );
                circle( color, objCenter, 5, RED, 2, 8, 0 );

                // --PASSENGERS DB UPDATE
                bool newPassenger = true;

                for(unsigned int i = 0; i < passengers.size(); i++) {

                    // If the passenger is near a known passenger assume they are the same one
                    if( abs(objCenter.x - passengers[i].getCurrentPoint().x) <= xNear &&
                        abs(objCenter.y - passengers[i].getCurrentPoint().y) <= yNear ) {

                        // Update coordinates
                        newPassenger = false;
                        passengers[i].updateCoords(objCenter);
                        // If the object is being tracked it means its active we don't want it to disappear
                        passengers[i].resetAge();

                        break;
                    }
                }

                // If wasn't near any known object is a new passenger
                if(newPassenger) {
                    Passenger p(pid, objCenter);
                    passengers.push_back(p);
                    pid++;
                }
            }
        }

        // For every passenger in passengers DB
        for(unsigned int i = 0; i < passengers.size(); i++) {
            
            if(passengers[i].getTracks().size() > 1) {

                // -- DRAWING PASSENGER TRAJECTORIES
                polylines(color, passengers[i].getTracks(), false, passengers[i].getTrackColor(),2);
                //putText(color, "Pid: " + to_string(passengers[i].getPid()), passengers[i].getCenter(), FONT_HERSHEY_SIMPLEX, 0.5, passengers[i].getTrackColor(), 2);

                // -- COUNTING
                // Up to down
                if( (passengers[i].getLastPoint().y < frame.rows/2 && passengers[i].getCurrentPoint().y >= frame.rows/2) ||
                    (passengers[i].getLastPoint().y <= frame.rows/2 && passengers[i].getCurrentPoint().y > frame.rows/2) ) {

                    cnt_out++;

                    // Visual feedback
                    circle(color, Point(color.cols - 20, 20), 8, RED, CV_FILLED);
                }

                // Down to up
                if( (passengers[i].getLastPoint().y > frame.rows/2 && passengers[i].getCurrentPoint().y <= frame.rows/2) ||
                    (passengers[i].getLastPoint().y >= frame.rows/2 && passengers[i].getCurrentPoint().y < frame.rows/2) ) {

                    cnt_in++;

                    // Visual feedback
                    circle(color, Point(color.cols - 20, 20), 8, GREEN, CV_FILLED);
                }
            }

            // --UPDATE PASSENGER STATS
            // Updating age
            passengers[i].updateAge();

            // If passenger is outside field of view update coords with last known position.
            // This prevents false positive counts.
            if(passengers[i].getAge() > 1)
                passengers[i].updateCoords(passengers[i].getCurrentPoint());

            // Removing older passengers
            if(passengers[i].getAge() > (maxPassengerAge * fps) )
                passengers.erase(passengers.begin() + i);
        }

        // Debugging
        // putText(color, "Tracked passengers: " + to_string(passengers.size()), Point(15,  15) , FONT_HERSHEY_SIMPLEX, 0.5, RED, 2);

        // --PRINTING INFORMATION
        putText(color, "FPS: " + to_string(fps), Point(0,  15) , FONT_HERSHEY_SIMPLEX, 0.5, RED, 2);

        // Horizontal line     
        line( color,
              Point(0,color.rows/2),            //Starting point of the line
              Point(color.cols,color.rows/2),   //Ending point of the line
              RED,                              //Color
              2,                                //Thickness
              8);                               //Linetype
        
        putText(color, "Count IN: "  + to_string(cnt_in), Point(0,color.rows - 30) , FONT_HERSHEY_SIMPLEX, 0.5, WHITE, 2);
        putText(color, "Count OUT: " + to_string(cnt_out), Point(0, color.rows - 10) , FONT_HERSHEY_SIMPLEX, 0.5, WHITE, 2);

        // --CALIBRATION TRACKBARS
        if(calibrationOn && displayColor) {
            createTrackbar("Threshold [centimeters]", "Color threadID: " + threadID, &thresholdCentimeters, 400);
            createTrackbar("Blur [matrix size]", "Color threadID: " + threadID, &blur_ksize, 100);
            createTrackbar("xNear [pixels]", "Color threadID: " + threadID, &xNear, ImageWidth);
            createTrackbar("yNear [pixels]", "Color threadID: " + threadID, &yNear, ImageHeight);
            createTrackbar("Area min [pixels^2]", "Color threadID: " + threadID, &areaMin, 100000);
            createTrackbar("Passenger age [seconds]", "Color threadID: " + threadID, &maxPassengerAge, 30);
            // Testing adaptive threshold
            createTrackbar("blockSize", "Color threadID: " + threadID, &blockSize, 800);
            createTrackbar("C", "Color threadID: " + threadID, &C_100, 1000);
        }

        // Show streams
        if(displayColor)
            imshow("Color threadID: " + threadID, color);

        if(displayFrame)
            imshow("Frame threadID: " + threadID, frame);
            
        if(displayRawDepth)
            imshow("RawDepth threadID: " + threadID, rawDepth);

        if(displayDepth)
            imshow("Distance threadID: " + threadID, depthColorMap);

        // -- SAVING VIDEOS
        if(saveVideo) {
            cvtColor(frame, frame, CV_GRAY2BGR);
            
            outputVideoFrame.write(frame);
            outputVideoDepth.write(depthColorMap);
            outputVideoColor.write(color);
        }

        // --PERFORMANCE ESTMATION
        high_resolution_clock::time_point t2 = high_resolution_clock::now(); //STOP

        if(firstLoop)
            loopTime = duration_cast<duration<double>>(t2 - t1);
        else {
            loopTime += duration_cast<duration<double>>(t2 - t1);
            loopTime = loopTime/2;
        }

        waitKey(1);

        firstLoop = false;

    }

    cout << "Loop execution time for " + (string)dev->get_name() + ": " << loopTime.count() << " seconds\n";

    // Close windows
    if(displayColor)
        destroyWindow("Color threadID: " + threadID);

    if(displayFrame)
        destroyWindow("Frame threadID: " + threadID);
        
    if(displayRawDepth)
        destroyWindow("RawDepth threadID: " + threadID);

    if(displayDepth)
        destroyWindow("Distance threadID: " + threadID);

    if(saveVideo) {
        outputVideoFrame.release();
        outputVideoDepth.release();
        outputVideoColor.release();
    }

    // Disable streams
    dev->stop();

    dev->disable_stream(rs::stream::color);
    dev->disable_stream(rs::stream::depth);

    return;
}


/* ****************************************************************************************************************
    NOTE:
    The depth matrix has a strange standard:
        0 = No depth data
        1 - 65535 = Depth where 1 is the nearest and 65535 is the farthest
    I want to have 65535 as the new No depth data values

    Furthermore, while displaying the depth data, I want to have the farthest object in black and the nearest in white
    this took some mental gymnastic to be implemented
**************************************************************************************************************** */
Mat RSPCN::getColorMap(Mat depthImage) {
    Mat depthColorMap;

    double min;
    double max;
    Point tmpMinLoc;
    Point tmpMaxLoc;

    int nearestVal;
    Point nearestLoc;

    int farthestVal;
    Point farthestLoc;

    // Saving farthest point
    minMaxLoc(depthImage, &min, &max, &tmpMinLoc, &tmpMaxLoc);
    farthestVal = max;
    farthestLoc = tmpMaxLoc;

    // If pixelValue == 0 set it to 65535( = 2^16 - 1)
    depthImage.setTo(65535, depthImage == NODATA);

    // Saving nearest point
    minMaxLoc(depthImage, &min, &max, &tmpMinLoc, &tmpMaxLoc);
    nearestVal = min;
    nearestLoc = tmpMinLoc;

    // Converts CV_16U to CV_8U using a scale factor of 255.0/ 65535
    depthImage.convertTo(depthImage, CV_8UC1, 255.0 / 65535);

    // Current situation: Nearest object => Black, Farthest object => White
    // We want to have  : Nearest object => White, Farthest object => Black
    depthImage = cv::Scalar::all(255) - depthImage;

    // Color map: Nearest object => Red, Farthest object => Blue
    equalizeHist( depthImage, depthImage );
    applyColorMap(depthImage, depthColorMap, COLORMAP_JET);

    // Highlight nearest and farthest pixel
    circle( depthColorMap, nearestLoc, 5, WHITE, 2, 8, 0 );
    putText(depthColorMap, "Nearest: " + to_string(nearestVal*scale) + " m", Point(0, depthColorMap.rows - 30), FONT_HERSHEY_SIMPLEX, 0.5, WHITE, 2);

    circle( depthColorMap, farthestLoc, 5, WHITE, 2, 8, 0 );
    putText(depthColorMap, "Farthest: " + to_string(farthestVal*scale) + " m", Point(0, depthColorMap.rows - 10), FONT_HERSHEY_SIMPLEX, 0.5, WHITE, 2);

    return depthColorMap;
}

Mat RSPCN::getFrame(Mat depthImage, int thresholdCentimeters) {
    Mat frame;

    // If depthImage(x,y) == NODATA, set it to 65535
    depthImage.setTo(65535, depthImage == NODATA);

    // Threshold only accepts CV_8U or CV_32F types
    depthImage.convertTo(depthImage, CV_32FC1);

    // Converting threshold from cm to pixel value
    int threshPixel = thresholdCentimeters / (100*scale);
    threshold(depthImage, depthImage, threshPixel, 65535, THRESH_BINARY);

    // Convert to CV_8U (lossy conversion)
    depthImage.convertTo(depthImage, CV_8UC1, 255.0 / 65535);

    // Invert b&w: white = foreground, black= background.
    frame = cv::Scalar::all(255) - depthImage;

    return frame;
}

void RSPCN::getExperimentalFrame(Mat depthImage, int blockSize, double C) {
    Mat frame;
    Mat imgLabels;
    Mat stats, centroids;

    // If depthImage(x,y) == NODATA, set it to 65535
    depthImage.setTo(65535, depthImage == NODATA);

    // Convert to CV_8U (lossy conversion)
    depthImage.convertTo(depthImage, CV_8UC1, 255.0 / 65535);

    // Invert b&w: white = foreground, black= background.
    frame = cv::Scalar::all(255) - depthImage;

    // ADAPTIVE THREHSOLD ATTEMPT
    /* ******************************************************************************************************************** */
    // equalizeHist(frame, frame);

    // if(blockSize % 2 != 1)
    // blockSize = 3;

    // adaptiveThreshold(frame, frame, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, blockSize, -1 * C);
    /* ******************************************************************************************************************** */

    // equalizeHist(frame, frame);

    // CONNECTED COMPONENTS REGIONAL MAXIMA
    /* ******************************************************************************************************************** */
    // int nLabels = connectedComponentsWithStats(frame, imgLabels, stats, centroids, 8, CV_16U);
    // 
    // depthImage.convertTo(imgLabels, CV_8UC1);
    // // equalizeHist(imgLabels, imgLabels);
    // cvtColor(imgLabels, imgLabels, CV_GRAY2BGR);

    // // Ignore 0 (and 1?) location label because it's the background
    // // for(int i = 2; i < nLabels; i++) {

    // //     // Hard threshold: minimum a rectangle 10x10
    // //     if( stats.at<int>(i, CC_STAT_WIDTH) > 10 || stats.at<int>(i, CC_STAT_HEIGHT) > 10) {
    // //         
    // //         // circle( imgLabels, Point((int)centroids.at<float>(i, 0), (int)centroids.at<float>(i, 1)), 5, RED, 2, 8, 0 );
    // //         circle( imgLabels, Point(stats.at<int>(i, CC_STAT_LEFT) + (int)stats.at<int>(i, CC_STAT_WIDTH)/2, stats.at<int>(i, CC_STAT_TOP) + (int)stats.at<int>(i, CC_STAT_HEIGHT)/2), 5, RED, 2, 8, 0 );
    // //         putText(imgLabels, "max: " + to_string(stats.at<int>(i, CC_STAT_MAX)), Point(stats.at<int>(i, CC_STAT_LEFT), stats.at<int>(i, CC_STAT_TOP)) , FONT_HERSHEY_SIMPLEX, 0.5, RED, 2);

    // //         rectangle( imgLabels,
    // //                    Point(stats.at<int>(i, CC_STAT_LEFT), stats.at<int>(i, CC_STAT_TOP)),
    // //                    Point(stats.at<int>(i, CC_STAT_LEFT) + stats.at<int>(i, CC_STAT_WIDTH), stats.at<int>(i, CC_STAT_TOP) + stats.at<int>(i, CC_STAT_HEIGHT)),
    // //                    GREEN,
    // //                    2,
    // //                    8,
    // //                    0 );
    // //     }
    // // }

    // for(int i = 2; i < nLabels; i++) {

    //     bool is_regional_min = true;

    //     for(int j = 0; j < nLabels; j++) {

    //         // Must not have a cc1 inside that have a max smaller than his
    //         if( isContained(stats.at<int>(i, CC_STAT_LEFT), stats.at<int>(i, CC_STAT_TOP), stats.at<int>(i, CC_STAT_WIDTH), stats.at<int>(i, CC_STAT_HEIGHT), 
    //             stats.at<int>(j, CC_STAT_LEFT), stats.at<int>(j, CC_STAT_TOP), stats.at<int>(j, CC_STAT_WIDTH), stats.at<int>(j, CC_STAT_HEIGHT) ) 
    //             && stats.at<int>(j, CC_STAT_MAX) > stats.at<int>(i, CC_STAT_MAX)) {

    //             is_regional_min = false;
    //             break;
    //         }
    //         
    //     }

    //     if(is_regional_min) {

    //         rectangle( imgLabels,
    //                    Point(stats.at<int>(i, CC_STAT_LEFT), stats.at<int>(i, CC_STAT_TOP)),
    //                    Point(stats.at<int>(i, CC_STAT_LEFT) + stats.at<int>(i, CC_STAT_WIDTH), stats.at<int>(i, CC_STAT_TOP) + stats.at<int>(i, CC_STAT_HEIGHT)),
    //                    GREEN,
    //                    2,
    //                    8,
    //                    0 );

    //     }
    //     
    // }
    //
    // putText(imgLabels, "nLabels: " + to_string(nLabels), Point(0,  15) , FONT_HERSHEY_SIMPLEX, 0.5, BLACK, 2);
    //
    /* ******************************************************************************************************************** */

    int levels = 8; 
    vector<isometrics> detectedObjects[8];

    Mat original = frame.clone();

    equalizeHist(frame, frame);
    // dilate(frame,frame, Mat(Size(6,6), CV_8UC1));

    cvtColor(original, original, CV_GRAY2BGR);

    for(int i = 1; i < levels; i++) {

        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;

        int thresh_level = (int)( 255 / levels ) * i;

        threshold(frame, frame, thresh_level, 255, THRESH_TOZERO);

        findContours(frame, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);

        for(unsigned int idx = 0; idx < contours.size(); idx++) {

            drawContours( original, contours, idx, Scalar(0,(int)( 255/levels) * i, 0), 2, 8, hierarchy, 0, Point(0,0) );

            Rect br = boundingRect(contours[idx]);
            Point objCenter = Point( (int)(br.x + br.width/2) ,(int)(br.y + br.height/2) );

            isometrics tmp;

            tmp.center = objCenter;
            tmp.boundingRectangle = br;

            detectedObjects[i].push_back(tmp);
        }

        contours.clear();
        hierarchy.clear();
    }

    for(int i = 1; i < levels; i++) {
        for(unsigned j = 0; j < detectedObjects[i].size(); j++) {
            // circle( original, detectedObjects[i][j].center, 5, RED, 2, 8, 0 );
            // rectangle( original, detectedObjects[i][j].boundingRectangle.tl(), detectedObjects[i][j].boundingRectangle.br(), RED, 2, 8, 0 );
            
            bool isRegionalMaxima = true;

            for(int k = i; k < levels; k++) {
                for(unsigned l = 0; l < detectedObjects[k].size(); l++) {
                    if( isContained( detectedObjects[i][j].center.x, detectedObjects[i][j].center.y, detectedObjects[i][j].boundingRectangle.width, detectedObjects[i][j].boundingRectangle.height,
                                     detectedObjects[k][l].center.x, detectedObjects[k][l].center.y, detectedObjects[k][l].boundingRectangle.width, detectedObjects[k][l].boundingRectangle.height) ){
                        isRegionalMaxima = false;
                        break;
                    }
                }
            }

            if(isRegionalMaxima){
                circle( original, detectedObjects[i][j].center, 5, RED, 2, 8, 0 );
            }
            else
                break;
        }
    }

    namedWindow("Experimental threadID: " + threadID,WINDOW_AUTOSIZE);
    imshow("Experimental threadID: " + threadID, original);

    return;
}

// Checks whether the ConnectedComponent2 is contained in the ConnectedComponent1
bool RSPCN::isContained(int cc1_x, int cc1_y, int cc1_width, int cc1_height, int cc2_x, int cc2_y, int cc2_width, int cc2_height ) {

    if( cc1_x == cc2_x && cc1_y == cc2_y && cc1_width == cc2_width && cc1_height == cc2_height )
        return false;

    cout << "Test" << endl;
    cout << "cc1_x: "<< cc1_x << endl;
    cout << "cc1_y: "<< cc1_y << endl;
    cout << "cc1_width: "<< cc1_width << endl;
    cout << "cc1_height: "<< cc1_height << endl;
    cout << "cc2_x: "<< cc2_x << endl;
    cout << "cc2_y: "<< cc2_y << endl;
    cout << "Result: " << (cc2_x < cc1_x + (int)(cc1_width/2) && cc2_y < cc1_y + (int)(cc1_height/2)) << endl;
    cout << endl;


    return (cc2_x < cc1_x + (int)(cc1_width/2) && cc2_y < cc1_y + (int)(cc1_height/2));
}

#endif

