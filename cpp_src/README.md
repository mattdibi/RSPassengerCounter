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

