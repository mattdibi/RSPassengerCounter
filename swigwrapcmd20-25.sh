#!/bin/bash
if [ -d "swig_output_2025" ]; then
  rm -r swig_output_2025/
fi

mkdir swig_output_2025
cp Main.java swig_output_2025/
swig -c++ -java -outdir swig_output_2025/ -o swig_output_2025/RSPCN_wrap.cpp RSPCN.i
cd swig_output_2025/

$CXX -c -fpic -std=c++11 -pthread ../RSPCN.cpp \
-lopencv_core \
-lopencv_highgui \
-lopencv_imgproc \
-lopencv_ml \
-lopencv_objdetect \
-lopencv_videoio \
-lopencv_video \
-lrealsense

$CXX -c -fpic -std=c++11  RSPCN_wrap.cpp -I/usr/lib/jvm/java-8-openjdk-amd64/include -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux

$CXX -std=c++11 -shared RSPCN.o RSPCN_wrap.o -o libRSPCN.so -lopencv_core \
-lopencv_highgui \
-lopencv_imgproc \
-lopencv_ml \
-lopencv_objdetect \
-lopencv_videoio \
-lopencv_video \
-lrealsense

