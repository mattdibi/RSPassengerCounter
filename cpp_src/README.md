## User guide

Requirements. A Linux system with:
* OpenCV 
* librealsense
* Cmake
* poky toolchain (generated with yocto project see [build guide](https://github.com/mattdibi/RSPassengerCounter/tree/master/build_config))

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
