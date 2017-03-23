%module RSPCNmodule

%{
#include "RSPCN.h"
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
    bool displayRawDepth = false;
    bool displayDepth = false;
    bool displayFrame = false;
    bool framerateStabilizationOn = true;
    bool saveVideo = false;
};
