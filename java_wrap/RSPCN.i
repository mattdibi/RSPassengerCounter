%module RSPCNmodule

%{
#include "../../cpp_src/passenger.h"

#include <iostream>
#include <string>
#include <chrono>
#include <thread>

#include <librealsense/rs.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
using namespace rs;
using namespace std::chrono;

// Camera type
#define R200 0
#define SR300 1

// Camera settings
#define IMAGE_WIDTH_R200     320
#define IMAGE_HEIGHT_R200    240
#define FRAMERATE_R200       60

#define IMAGE_WIDTH_SR300     640
#define IMAGE_HEIGHT_SR300    480
#define FRAMERATE_SR300       30

// Constants
#define MAXRGBVAL 255
#define NODATA 0

#define RED   Scalar(0,0,255)
#define BLUE  Scalar(255,0,0)
#define GREEN Scalar(0,255,0)
#define WHITE Scalar(255,255,255)
#define BLACK Scalar(0,0,0)

// Calibration starting values
#define MAX_RANGE_CM 43         // [centimeters]
#define BLUR_KSIZE 10           // [matrix size]
#define AREA_MIN 10000          // [pixels^2]: This depends on the camera distance from the passengers
#define X_NEAR 40               // [pixels]
#define Y_NEAR 90               // [pixels]
#define MAX_PASSENGER_AGE 2     // [seconds]

#define MAX_1PASS_AREA 60000    // [pixels^2]
#define MAX_2PASS_AREA 90000    // [pixels^2]

class RSPCN {

  public:

    // Constructor
    RSPCN(int deviceIdx);
    ~RSPCN() { thread_.join(); }

    // Selectors
    string getThreadID(){return threadID;};

    float getDeviceScale(){return scale;};
    string getDeviceName(){return dev->get_name();};
    string getDeviceSerial(){return dev->get_serial();};
    string getDeviceFirmware(){return dev->get_firmware_version();};

    int getCountIn(){return cnt_in;};
    int getCountOut(){return cnt_out;};

    // Setters
    void setCalibration(bool value) {calibrationOn = value; return;};
    void setDisplayColor(bool value) {displayColor = value; return;};
    void setDisplayDepth(bool value) {displayDepth = value; return;};
    void setDisplayRawDepth(bool value) {displayRawDepth = value; return;};
    void setDisplayFrame(bool value) {displayFrame = value; return;};
    void setFramerateStabilization(bool value) {framerateStabilizationOn = value; return;};
    void setCameraPresets(int value);
    void setSaveVideo(bool value) {saveVideo = value; return;};

    // Methods
    void start();
    void count();
    void resetCounters(){cnt_in = 0; cnt_out = 0; return;};
    void stop(){halt = true;};

    void toggleCalibration();
    void toggleDisplayColor();
    void toggleDisplayDepth();
    void toggleDisplayRawDepth();
    void toggleDisplayFrame();
    void toggleFrameRateStabilization(){framerateStabilizationOn = !framerateStabilizationOn; return;};

  private:
    context ctx;
    device * dev;

    std::thread thread_;
    string threadID;

    bool halt = false;

    // Camera type
    bool cameraDevice;

    // Camera options
    int ImageWidth;
    int ImageHeight;
    int CameraFramerate;

    // Camera scale
    float scale;

    // Passenger counters
    int cnt_in  = 0;
    int cnt_out = 0;

    // Passengers tracker
    int pid = 0;
    vector<Passenger> passengers;

    // Options
    bool calibrationOn = false;
    bool displayColor = false;
    bool displayRawDepth = false;
    bool displayDepth = false;
    bool displayFrame = false;
    bool framerateStabilizationOn = true;
    bool saveVideo = false;
};
%}

/* Let's just grab the original header file here */
class RSPCN {

  public:

    // Constructor
    RSPCN(int deviceIdx);
    ~RSPCN() { thread_.join(); }

    // Selectors
    string getThreadID(){return threadID;};

    float getDeviceScale(){return scale;};
    string getDeviceName(){return dev->get_name();};
    string getDeviceSerial(){return dev->get_serial();};
    string getDeviceFirmware(){return dev->get_firmware_version();};

    int getCountIn(){return cnt_in;};
    int getCountOut(){return cnt_out;};

    // Setters
    void setCalibration(bool value) {calibrationOn = value; return;};
    void setDisplayColor(bool value) {displayColor = value; return;};
    void setDisplayDepth(bool value) {displayDepth = value; return;};
    void setDisplayRawDepth(bool value) {displayRawDepth = value; return;};
    void setDisplayFrame(bool value) {displayFrame = value; return;};
    void setFramerateStabilization(bool value) {framerateStabilizationOn = value; return;};
    void setCameraPresets(int value);
    void setSaveVideo(bool value) {saveVideo = value; return;};

    // Methods
    void start();
    void count();
    void resetCounters(){cnt_in = 0; cnt_out = 0; return;};
    void stop(){halt = true;};

    void toggleCalibration();
    void toggleDisplayColor();
    void toggleDisplayDepth();
    void toggleDisplayRawDepth();
    void toggleDisplayFrame();
    void toggleFrameRateStabilization(){framerateStabilizationOn = !framerateStabilizationOn; return;};

  private:
    context ctx;
    device * dev;

    std::thread thread_;
    string threadID;

    bool halt = false;

    // Camera type
    bool cameraDevice;

    // Camera options
    int ImageWidth;
    int ImageHeight;
    int CameraFramerate;

    // Camera scale
    float scale;

    // Passenger counters
    int cnt_in  = 0;
    int cnt_out = 0;

    // Passengers tracker
    int pid = 0;
    vector<Passenger> passengers;

    // Options
    bool calibrationOn = false;
    bool displayColor = false;
    bool displayRawDepth = false;
    bool displayDepth = false;
    bool displayFrame = false;
    bool framerateStabilizationOn = true;
    bool saveVideo = false;
};
