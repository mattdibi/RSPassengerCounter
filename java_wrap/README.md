## User guide
Requirements. A Linux system with:
* OpenCV 
* librealsense
* JAVA SDK (JDK 8) 
* SWIG
* poky toolchain (generated with yocto project see [build guide](https://github.com/mattdibi/RSPassengerCounter/tree/master/build_config))

**Build JAVA wrap version** 
```sh
$ sh swigwrapcmd.sh
$ cd swig_output/
$ javac *.java
$ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path/to/swig_output
$ java Main
```

**Build JAVA wrap poky version**
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

### Note
Please note that this wrapper uses an obsolete version of the cpp_src code. For the latest features use the java_src version.
    
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
