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
