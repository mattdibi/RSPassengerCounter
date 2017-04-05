## User guide

Requirements. A Linux system with:
* OpenCV 
* librealsense
* JAVA SDK (JDK 8) 

**Use the compile script**
```sh
$ sh JavaCPPCompile.sh
```

**Use OpenCV Java API with the installed JDK**  
Once connected to ReliGATE:
```sh
javac -cp /usr/share/OpenCV/java/opencv-310.jar:. <your source>.java 
java -cp /usr/share/OpenCV/java/opencv-310.jar:. -Djava.library.path=/usr/share/OpenCV/java/ <your source>
```
