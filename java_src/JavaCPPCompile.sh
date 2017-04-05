javac -cp jar_files/javacpp.jar:jar_files/librealsense-platform.jar:/usr/share/java/opencv.jar TestConnection.java
java -jar jar_files/javacpp.jar TestConnection
java -cp jar_files/javacpp.jar:jar_files/librealsense-platform.jar:/usr/share/java/opencv.jar:. -Djava.library.path=/usr/lib/jni/ TestConnection

