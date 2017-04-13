## User guide

Requirements. A Linux system with:
* OpenCV 
* librealsense
* JAVA SDK (JDK 8) 

The jar_files folder contains a subset of the binary files provided by JavaCV and JavaCPP-presets
projects. These are needed to use the RealSense cameras.

**Use the compile script**
```sh
$ sh Compile.sh
$ sh Run.sh
```
### Execution modes
```
> M: bare metal mode.
> N: normal mode.
> V: video recording mode.
> B: video recording in bare metal mode.
```

### Runtime commands
```
> q: exit program
> r: resetting counters
> c: get passenger count
> p: set camera preset
> t: set threshold centimeters
> a: set max passenger age
> b: set blur kernel size
> x: set xNear
> y: set yNear
> h: display this help message
```
## Note
Please note that this project is not using the installed OpenCV Java API, instead it's using JavaCV and JavaCPP. The files needed to compile
and run the code correctly are included in the jar_files folder and referenced by the Compile and Run scripts.

## Use OpenCV Java API with the installed JDK
If you want to use the included OpenCV Java API, once connected to ReliGATE:
```sh
javac -cp /usr/share/OpenCV/java/opencv-310.jar:. <your source>.java 
java -cp /usr/share/OpenCV/java/opencv-310.jar:. -Djava.library.path=/usr/share/OpenCV/java/ <your source>
```
### Additional informations
* [ JavaCV Repository example ](https://github.com/bytedeco/javacv/blob/master/src/main/java/org/bytedeco/javacv/RealSenseFrameGrabber.java)
* [ JavaCV Repository ](https://github.com/bytedeco/javacv)
* [ JavaCPP Repository ](https://github.com/bytedeco/javacpp)
* [ JavaCPP-Presets Repository ](https://github.com/bytedeco/javacpp-presets)
