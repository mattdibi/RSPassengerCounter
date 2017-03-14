# RealSense Passenger Counter (RSPCN)
This is going to be my Electronics Engineering Master's Thesis.

### Goal
Developing a Passenger Counter (PCN) in a transportation environment using OpenCV and its hardware acceleration capabilities on different hardware platforms. In this implementation I am using the Intel RealSense cameras.

### Hardware
Platform: 
* Eurotech ReliGATE 20-25 (Intel E3827 Atom Processor)

Video acquisition:
* Intel RealSense SR300
* Intel RealSense R200

### Tools
For the development I'm going to use:
* Yocto Project
* OpenCV
* Librealsense

### Performance
Performance achived on Eurotech platform:
* Intel RealSense SR300 @640x480 30FPS = [30; 400] cm range
* Intel RealSense R200  @320x240 60FPS = [10; 150] cm range

### Options
```sh
              - Without arguments: it opens the default webcam and captures the input stream.
-d            - Depth mode: it display the depth color map. WARNING: may decrese performance
-s <filename> - Capture mode: it saves all the stream on file.
-c            - Calibration mode: it opens the default webcam and display calibration trackbars.
-x            - Disable framerate stabilization. The program will run at max FPS possible.
-h            - Display help.
```