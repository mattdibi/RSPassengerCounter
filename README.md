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
* [JavaCPP](https://github.com/bytedeco/javacpp) and its presets for the librealsense library

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

### Software stack

```
  ──────────                  ────────────────   ==> (RSPCN ver. OSGi)
 │  OpenCV  │                │  librealsense  │ 
 │OSGi bund.│                │  OSGi bundle   │ 
  ──────────   ────────────   ────────────────   ──────────────────   ==> (RSPCN ver. Java)
 │  OpenCV  │ │   OpenCV   │ │ Java interface │ │  OSGi Framework  │
 │ Java API │ │ Python API │ │                │ │    (Equinox)     │
  ─────────────────────────   ────────────────   ──────────────────   ==> (RSPCN ver. C++ )
 │   OpenCV 3.1 Library    │ │  librealsense  │ │     JRE/JDK      │
 │     (native library)    │ │   (driver RS)  │ │    openjdk-8     │
  ───────────────────────────────────────────────────────────────── 
 │        Custom linux distribution (Poky w/ Yocto Project)        │
  ───────────────────────────────────────────────────────────────── 
 │                       ReliGate 20-25                            │
  ───────────────────────────────────────────────────────────────── 
```

### Roadmap

- [X] RSPCN C++ version  => Release v2.0
- [X] RSPCN Java version => Release v3.0
- [ ] RSPCN OSGi version 

### Improvements

- build_config
  - Custom recipe to auto install needed jar files in the root directory

- cpp_src
  - Improve counting algorithm.
  - Improve tracking algorithm using Haar Cascade Classifiers

- java_src
  - Add multicamera support
  - Fix color stream framerate
  - Follow improvements of the cpp version

- osgi_src
  - Eclipse configuration to automatically launch the program on the target platform from the host machine

- docs
  - Add photos in the README file
