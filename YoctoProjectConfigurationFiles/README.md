# Yocto build guide

## Step 1: Clone needed repositories
```sh
$ git clone git://git.yoctoproject.org/poky
$ cd ~/poky
$ git clone git://git.yoctoproject.org/meta-intel
$ git clone git://git.openembedded.org/meta-openembedded
$ git clone https://github.com/IntelRealSense/meta-intel-realsense.git
$ git clone git://git.yoctoproject.org/meta-java
$ git clone git://git.yoctoproject.org/meta-oracle-java
```

**Important**: Set them to track morty branch

```sh
$ git checkout morty
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
$ bitbake-layers add-layer "$HOME/poky/meta-oracle-java"
```

## Step 3: Modify conf files
* First you need to modify **local.conf**. Use the one provided in this repository
* Then add the **auto.conf** file needed for the librealsense library. Use the one provided in this repository

## Launch the build
Use the command:

```sh
$ bitbake core-image-sato
```

to launch the build process. We want Yocto to download the openjdk7 sources so that they can be patched manually
follwing the instruction in Step 4. We'll need to wait for it to fail the build so that we can edit those files. 

## Step 4: Patch openjdk
[Here](https://bugzilla.opensuse.org/attachment.cgi?id=678295&action=diff) are reported the needed modification to properly build
the image. It will give an Hash error but the image will be correctly built.
I have included the pathched files in this repository in case they are needed in the future.
Installation paths:
* $HOME/poky/build/tmp/work/corei7-64-poky-linux/openjdk-7-jre/99b00-2.6.5-r6.1/icedtea-2.6.5/build/openjdk/hotspot/src/share/vm/gc_implementation/g1/concurrentMark.cpp
* $HOME/poky/build/tmp/work/corei7-64-poky-linux/openjdk-7-jre/99b00-2.6.5-r6.1/icedtea-2.6.5/build/openjdk/hotspot//src/os/posix/vm/os_posix.cpp
* $HOME/poky/build/tmp/work/corei7-64-poky-linux/openjdk-7-jre/99b00-2.6.5-r6.1/icedtea-2.6.5/build/openjdk/hotspot/src/share/vm/prims/unsafe.cpp
* $HOME/poky/build/tmp/work/corei7-64-poky-linux/openjdk-7-jre/99b00-2.6.5-r6.1/icedtea-2.6.5/build/openjdk/hotspot/src/share/vm/code/dependencies.hpp
* $HOME/poky/build/tmp/work/corei7-64-poky-linux/openjdk-7-jre/99b00-2.6.5-r6.1/icedtea-2.6.5/build/openjdk/hotspot/src/share/vm/oops/cpCacheOop.hpp


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
├── meta-oracle-java
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
meta-oracle-java  = "morty:f44365f02b283c3fb362dc99e2e996d3f11e356e"
```

## Step 5: Build
Build the image:
```sh
$ bitbake -k core-image-sato
```

Output files will be available in $HOME/poky/build/tmp/deploy/images/intel-corei7-64/ folder.

**Note**: It will output an error. Ignore it since the image will correctly be built.

Build the cross-compiler installer:
```sh
$ bitbake core-image-sato -c populate_sdk
```

Output files will be available in $HOME/poky/build/tmp/deploy/sdk/ folder

Burn the image:
```sh
$ sudo dd if=tmp/deploy/images/intel-corei7-64/core-image-base-intel-
corei7-64.wic of=TARGET_DEVICE status=progress
```

## Additional informations
* Example project installing JAVA: [link](http://wiki.hioproject.org/index.php?title=OpenHAB:_WeMo_Switch)
* Stackoverflow question about Java installation on Yocto build error: [link](http://stackoverflow.com/questions/43093838/java-installation-error-on-yocto-build)
  * Solution to said error: apparently the bug is known and was patched: [source](https://bugzilla.opensuse.org/show_bug.cgi?id=981625)

**Extra**: Stackoverflow question about installing libc6 lib [link](http://stackoverflow.com/questions/43074547/libc6-i386-installation-on-yocto-build/43076771#43076771)
