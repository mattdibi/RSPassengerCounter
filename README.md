# RealSense Passenger Counter (RSPCN)
This is going to be my Electronics Engineering Master's Thesis.

### Goal
Developing a Passenger Counter (PCN) in a transportation environment using OpenCV and its hardware acceleration capabilities on different hardware platforms. In this implementation I am using the Intel RealSense cameras.

### Hardware
Target platform: 
* Eurotech ReliGATE 20-25 (Intel E3827 Atom Processor)

Video acquisition:
* Intel RealSense SR300
* Intel RealSense R200

### Tools
For the development I've used:
* [OpenCV](http://opencv.org/)
* Intel RealSense Library: [librealsense](https://github.com/IntelRealSense/librealsense)
* Simplified Wrapper and Interface Generator: [SWIG](http://www.swig.org/)

To build the ReliGATE 20-25 image I've used:
* [Yocto Project](https://www.yoctoproject.org/)
    * [poky](http://git.yoctoproject.org/cgit.cgi/poky): base repository
    * [meta-intel](http://git.yoctoproject.org/cgit.cgi/meta-intel): layer for targeting the Intel Atom processor
    * [meta-openembedded](https://github.com/openembedded/meta-openembedded): layer containing OpenCV recipes
    * [meta-intel-realsense](https://github.com/IntelRealSense/meta-intel-realsense.git): layer containig librealsense library
    * [meta-java](http://git.yoctoproject.org/cgit/cgit.cgi/meta-java): layer containing the Java Run-time Environment

### Performance
Performance achived on Eurotech platform:
* Intel RealSense SR300 @640x480 30FPS = [30; 400] cm range
* Intel RealSense R200  @320x240 60FPS = [10; 150] cm range

### Options
```sh
    - Without arguments: it opens the default webcam and captures the input stream.
-s  - Capture mode: it saves the color stream on file.
```

### Runtime commands
```
> 0 - 5: selecting camera presets
> r: resetting counters
> p: get passenger count
> c: toggle display color
> C: toggle display calibration
> d: toggle display depth view
> D: toggle display raw depth view
> f: toggle display frame view
> s: toggle frame rate stabilization
> q: exit program
> h: display this help message
```

### User guide
Requirements. A Linux system with:
* OpenCV 
* librealsense
* JAVA SDK (JDK 8) 
* SWIG
* Cmake
* poky toolchain (generated with yocto project see [build guide](https://github.com/mattdibi/RSPassengerCounter/tree/master/YoctoProjectConfigurationFiles))


**Build C++ version**
```sh
$ cmake .
$ make
$ ./RSPCN
```

**Build C++ poky version**
```sh
$ ./X-COMPILE-OCV
```

**Build JAVA version** 
```sh
$ sh swigwrapcmd.sh
$ cd swig_output/
$ javac *.java
$ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path/to/swig_output
$ java Main
```

**Build JAVA poky version**
```sh
$ source /opt/poky/2.2.1/environment-setup-corei7-64-poky-linux
$ sh swigwrapcmd20-25.sh
$ scp swig_output_20-25/ root@<ReliGATE 20-25 IP address>:
```
Once connected to ReliGATE:
```sh
$ cd swig_output_2025/
$ javac *.java
$ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path/to/swig_output_2025
$ java Main
```
**Use OpenCV Java API with the installed JDK**  
Once connected to ReliGATE:
```sh
javac -cp /usr/share/OpenCV/java/opencv-310.jar:. <your source>.java 
java -cp /usr/share/OpenCV/java/opencv-310.jar:. -Djava.library.path=/usr/share/OpenCV/java/ <your source>
```
