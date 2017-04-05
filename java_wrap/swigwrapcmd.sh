#!/bin/bash
if [ -d "swig_output" ]; then
  rm -r swig_output/
fi

mkdir swig_output
cp Main.java swig_output/
swig -c++ -java -outdir swig_output/ -o swig_output/RSPCN_wrap.cpp RSPCN.i
cd swig_output/

g++ -c -fpic -std=c++11 -pthread -I../../cpp_src ../../cpp_src/RSPCN.cpp \
-lopencv_core \
-lopencv_highgui \
-lopencv_imgproc \
-lopencv_ml \
-lopencv_objdetect \
-lopencv_videoio \
-lopencv_video \
-lrealsense

g++ -c -fpic -std=c++11  RSPCN_wrap.cpp -I/usr/lib/jvm/java-8-openjdk-amd64/include -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux

g++ -std=c++11 -shared RSPCN.o RSPCN_wrap.o -o libRSPCN.so -lopencv_core \
-lopencv_highgui \
-lopencv_imgproc \
-lopencv_ml \
-lopencv_objdetect \
-lopencv_videoio \
-lopencv_video \
-lrealsense

