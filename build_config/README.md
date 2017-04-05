# Yocto build guide

## Step 1: Clone needed repositories
```sh
$ git clone git://git.yoctoproject.org/poky -b morty
$ cd ~/poky
$ git clone git://git.yoctoproject.org/meta-intel -b morty
$ git clone git://git.openembedded.org/meta-openembedded -b morty
$ git clone https://github.com/IntelRealSense/meta-intel-realsense.git -b morty
$ git clone git://git.yoctoproject.org/meta-java
```

**Important**: Set meta-java layer to the following commit

 ```sh
$ git reset --hard 67e48693501bddb80745b9735b7b3d4d28dce9a1 
```

## Step 2: Build environment
```sh
$ source oe-init-build-env
```

Start adding needed layers
```sh
$ cd $HOME/poky/build
$ bitbake-layers add-layer "$HOME/poky/meta-intel"
$ bitbake-layers add-layer "$HOME/poky/meta-openebedded/meta-oe"
$ bitbake-layers add-layer "$HOME/poky/meta-intel-realsense"
$ bitbake-layers add-layer "$HOME/poky/meta-java"
```

## Step 3: Modify conf files
* First you need to modify **local.conf**. Use the one provided in this repository
* Then add the **auto.conf** file needed for the librealsense library. 

Use the ones provided in this repository

## Step 4: Modify opencv recipe
Since we have to use the morty branch of the repository because of a compatibility issue of the realsense library, we cannot
access the latest version of the opencv recipe which support openjdk as a Java Run-Time Environment.
Referencing [that version](https://github.com/openembedded/meta-openembedded/commit/39d2e1b70a963835b707ccbd80dae6c34205e7a2) we can modify our recipe to add what we need.

Apply the following modifications to **$HOME/poky/meta-openembedded/meta-oe/recipes-support/opencv/opencv_3.1.bb**
```diff
...
- PACKAGECONFIG ??= "eigen jpeg png tiff v4l libv4l gstreamer samples tbb gphoto2 \
+ PACKAGECONFIG ??= "eigen jpeg png tiff v4l libv4l gstreamer samples tbb java gphoto2 \
    ${@bb.utils.contains("DISTRO_FEATURES", "x11", "gtk", "", d)} \
    ${@bb.utils.contains("LICENSE_FLAGS_WHITELIST", "commercial", "libav", "", d)}"
...
PACKAGECONFIG[jasper] = "-DWITH_JASPER=ON,-DWITH_JASPER=OFF,jasper,"
+ PACKAGECONFIG[java] = "-DJAVA_INCLUDE_PATH=${JAVA_HOME}/include -DJAVA_INCLUDE_PATH2=${JAVA_HOME}/include/linux -DJAVA_AWT_INCLUDE_PATH=${JAVA_HOME}/include -DJAVA_AWT_LIBRARY=${JAVA_HOME}/lib/amd64/libjawt.so -DJAVA_JVM_LIBRARY=${JAVA_HOME}/lib/amd64/server/libjvm.so,,ant-native fastjar-native openjdk-8-native,"
PACKAGECONFIG[jpeg] = "-DWITH_JPEG=ON,-DWITH_JPEG=OFF,jpeg,"
...
PACKAGECONFIG[opencl] = "-DWITH_OPENCL=ON,-DWITH_OPENCL=OFF,opencl-headers,"
- PACKAGECONFIG[oracle-java] = "-DJAVA_INCLUDE_PATH=${JAVA_HOME}/include -DJAVA_INCLUDE_PATH2=${JAVA_HOME}/include/linux -DJAVA_AWT_INCLUDE_PATH=${JAVA_HOME}/include -DJAVA_AWT_LIBRARY=${JAVA_HOME}/lib/amd64/libjawt.so -DJAVA_JVM_LIBRARY=${JAVA_HOME}/lib/amd64/server/libjvm.so,,ant-native oracle-jse-jdk oracle-jse-jdk-native,"
+ PACKAGECONFIG[oracle-java] = "-DJAVA_INCLUDE_PATH=${ORACLE_JAVA_HOME}/include -DJAVA_INCLUDE_PATH2=${ORACLE_JAVA_HOME}/include/linux -DJAVA_AWT_INCLUDE_PATH=${ORACLE_JAVA_HOME}/include -DJAVA_AWT_LIBRARY=${ORACLE_JAVA_HOME}/lib/amd64/libjawt.so -DJAVA_JVM_LIBRARY=${ORACLE_JAVA_HOME}/lib/amd64/server/libjvm.so,,ant-native oracle-jse-jdk oracle-jse-jdk-native,"
PACKAGECONFIG[png] = "-DWITH_PNG=ON,-DWITH_PNG=OFF,libpng,"
...
export PYTHON="${STAGING_BINDIR_NATIVE}/python"
- export JAVA_HOME="${STAGING_DIR_NATIVE}/usr/bin/java"
+ export ORACLE_JAVA_HOME="${STAGING_DIR_NATIVE}/usr/bin/java"
+ export JAVA_HOME="${STAGING_DIR_NATIVE}/usr/lib/jvm/openjdk-8-native"
export ANT_DIR="${STAGING_DIR_NATIVE}/usr/share/ant/"
...
PACKAGES += "${@bb.utils.contains('PACKAGECONFIG', 'oracle-java', '${PN}-java-dbg ${PN}-java', '', d)} \
+   ${@bb.utils.contains('PACKAGECONFIG', 'java', '${PN}-java-dbg ${PN}-java', '', d)} \ 
    ${PN}-samples-dbg ${PN}-samples ${PN}-apps python-opencv"
...
```

## Step 5: Launch the build
Use the command:
```sh
$ bitbake core-image-sato
```
**Note:** Depending on your host system configuration there may be some build problems. Please refer to the 
[troubleshooting section](https://github.com/mattdibi/RSPassengerCounter/tree/master/build_config#troubleshooting).

## Resulting folder structure

```sh
poky
├── bitbake
├── build
│   ├── bitbake.lock
│   ├── cache
│   ├── conf
│   │   ├── auto.conf
│   │   ├── bblayers.conf
│   │   ├── local.conf
│   │   ├── sanity_info
│   │   └── templateconf.cfg
│   ├── downloads
│   ├── sstate-cache
│   └── tmp
├── documentation
├── LICENSE
├── meta
├── meta-intel
├── meta-intel-realsense
├── meta-openembedded
├── meta-java
├── meta-poky
├── meta-selftest
├── meta-skeleton
├── meta-yocto
├── meta-yocto-bsp
├── oe-init-build-env
├── oe-init-build-env-memres
├── README
├── README.hardware
└── scripts
```

## Resulting build configuration
```
Build Configuration:
BB_VERSION        = "1.32.0"
BUILD_SYS         = "x86_64-linux"
NATIVELSBSTRING   = "universal-4.8"
TARGET_SYS        = "x86_64-poky-linux"
MACHINE           = "intel-corei7-64"
DISTRO            = "poky"
DISTRO_VERSION    = "2.2.1"
TUNE_FEATURES     = "m64 corei7"
TARGET_FPU        = ""
meta              
meta-poky         
meta-yocto-bsp    = "morty:924e576b8930fd2268d85f0b151e5f68a3c2afce"
meta-intel        = "morty:6add41510412ca196efb3e4f949d403a8b6f35d7"
meta-oe           = "morty:fe5c83312de11e80b85680ef237f8acb04b4b26e"
meta-intel-realsense = "morty:2c0dfe9690d2871214fba9c1c32980a5eb89a421"
meta-java         = "master:67e48693501bddb80745b9735b7b3d4d28dce9a1"
```

## Useful commands 

##### Build the image:
```sh
$ bitbake core-image-sato
```

Output files will be available in **$HOME/poky/build/tmp/deploy/images/intel-corei7-64/** folder.

##### Build the cross-compiler installer:
```sh
$ bitbake core-image-sato -c populate_sdk
```

Output files will be available in **$HOME/poky/build/tmp/deploy/sdk/** folder

##### Burn the image:
```sh
$ sudo dd if=tmp/deploy/images/intel-corei7-64/core-image-base-intel-
corei7-64.wic of=TARGET_DEVICE status=progress
```

## Troubleshooting
There's a known issue with some older version of C++ compilers.
In the error **"unrecognized command line option -fno-lifetime-dse"** pops up you need to
modify **$HOME/poky/meta-java/recipes-core/openjdk/openjdk-8-common.inc line 224** as following:
```diff
# GCC 6 sets the default C++ standard to C++14 and introduces dead store
# elimination by default. OpenJDK 8 is not ready for either of these
# changes.
- FLAGS_GCC6 = "-fno-lifetime-dse -fno-delete-null-pointer-checks"
+ FLAGS_GCC6 = "-fno-delete-null-pointer-checks"
```
### Additional informations
* Example project installing JAVA: [link](http://wiki.hioproject.org/index.php?title=OpenHAB:_WeMo_Switch)
* Stackoverflow question about Java installation on Yocto build error: [link](http://stackoverflow.com/questions/43093838/java-installation-error-on-yocto-build)
  * Solution to said error: apparently the bug is known and was patched: [source](https://bugzilla.opensuse.org/show_bug.cgi?id=981625)

**Extra**: Stackoverflow question about installing libc6 lib [link](http://stackoverflow.com/questions/43074547/libc6-i386-installation-on-yocto-build/43076771#43076771)
