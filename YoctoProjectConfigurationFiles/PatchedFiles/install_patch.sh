if [ $# -eq 0 ]; then
  echo "No arguments supplied"
  echo "This script is going to install the openjdk-7 patch. Please insert your Yocto build path."
  echo "Supported format: /path/to/poky/folder"
  exit 1
fi

echo "Input: $1"

if [ -d "$1/poky/filec" ]; then
  echo "Directory filec found."
    
  if ! [ -d "$1/poky/filec/tmp/work/corei7-64-poky-linux/openjdk-7-jre/99b00-2.6.5-r6.1/icedtea-2.6.5/build/openjdk/hotspot/src/" ] ; then
    echo "No openjdk-7 source folder found. Run build then install patch"
    exit 1
  else
    echo "Installing patch..."
    cp concurrentMark.cpp $1/poky/filec/tmp/work/corei7-64-poky-linux/openjdk-7-jre/99b00-2.6.5-r6.1/icedtea-2.6.5/build/openjdk/hotspot/src/share/vm/gc_implementation/g1/
    cp os_posix.cpp $1/poky/filec/tmp/work/corei7-64-poky-linux/openjdk-7-jre/99b00-2.6.5-r6.1/icedtea-2.6.5/build/openjdk/hotspot//src/os/posix/vm/
    cp unsafe.cpp $1/poky/filec/tmp/work/corei7-64-poky-linux/openjdk-7-jre/99b00-2.6.5-r6.1/icedtea-2.6.5/build/openjdk/hotspot/src/share/vm/prims/
    cp dependencies.hpp $1/poky/filec/tmp/work/corei7-64-poky-linux/openjdk-7-jre/99b00-2.6.5-r6.1/icedtea-2.6.5/build/openjdk/hotspot/src/share/vm/code/
    cp cpCacheOop.hpp $1/poky/filec/tmp/work/corei7-64-poky-linux/openjdk-7-jre/99b00-2.6.5-r6.1/icedtea-2.6.5/build/openjdk/hotspot/src/share/vm/oops/
  fi

elif [ -d "$1/poky/build" ]; then
  echo "Directory build found."

  if ! [ -d "$1/poky/build/tmp/work/corei7-64-poky-linux/openjdk-7-jre/99b00-2.6.5-r6.1/icedtea-2.6.5/build/openjdk/hotspot/src/" ] ; then
    echo "No openjdk-7 source folder found. Run build then install patch"
    exit 1
  else
    echo "Installing patch..."
    cp concurrentMark.cpp $1/poky/build/tmp/work/corei7-64-poky-linux/openjdk-7-jre/99b00-2.6.5-r6.1/icedtea-2.6.5/build/openjdk/hotspot/src/share/vm/gc_implementation/g1/
    cp os_posix.cpp $1/poky/build/tmp/work/corei7-64-poky-linux/openjdk-7-jre/99b00-2.6.5-r6.1/icedtea-2.6.5/build/openjdk/hotspot//src/os/posix/vm/
    cp unsafe.cpp $1/poky/build/tmp/work/corei7-64-poky-linux/openjdk-7-jre/99b00-2.6.5-r6.1/icedtea-2.6.5/build/openjdk/hotspot/src/share/vm/prims/
    cp dependencies.hpp $1/poky/build/tmp/work/corei7-64-poky-linux/openjdk-7-jre/99b00-2.6.5-r6.1/icedtea-2.6.5/build/openjdk/hotspot/src/share/vm/code/
    cp cpCacheOop.hpp $1/poky/build/tmp/work/corei7-64-poky-linux/openjdk-7-jre/99b00-2.6.5-r6.1/icedtea-2.6.5/build/openjdk/hotspot/src/share/vm/oops/
  fi

else
  echo "No build directory found. Chech path"
fi
