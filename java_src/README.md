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

**Use OpenCV Java API with the installed JDK**  
Once connected to ReliGATE:
```sh
javac -cp /usr/share/OpenCV/java/opencv-310.jar:. <your source>.java 
java -cp /usr/share/OpenCV/java/opencv-310.jar:. -Djava.library.path=/usr/share/OpenCV/java/ <your source>
```

### Additional informations
* [ JavaCV Repository example ](https://github.com/bytedeco/javacv/blob/master/src/main/java/org/bytedeco/javacv/RealSenseFrameGrabber.java)
* [ JavaCV Repository ](https://github.com/bytedeco/javacv)
* [ JavaCPP Repository ](https://github.com/bytedeco/javacpp)
* [ JavaCPP-Presets Repository ](https://github.com/bytedeco/javacpp-presets)
