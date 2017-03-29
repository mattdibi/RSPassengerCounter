# Yocto build guide

## Clone needed repositories
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

## Build environment
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

## Modify conf files
* First you need to modify **local.conf**. Use the one provided in this repository
* Then add the **auto.conf** file needed for the librealsense library. Use the one provided in this repository

**Note**: Seems like the PREFERRED PROVIDER for openjdk-7-jre must be oracle-jse-jre

## Patch openjdk
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

## Build
Build the image:
```sh
$ bitbake -k core-image-sato
```

Output files will be available in $HOME/poky/build/tmp/deploy/images/intel-corei7-64/ folder

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

## Additional informations/WIP
* Stackoverflow question about installing libc6 lib:[link](http://stackoverflow.com/questions/43074547/libc6-i386-installation-on-yocto-build/43076771#43076771)
* Example project installing JAVA: [link](http://wiki.hioproject.org/index.php?title=OpenHAB:_WeMo_Switch)
* Stackoverflow question about Java installation on Yocto build error: [link](http://stackoverflow.com/questions/43093838/java-installation-error-on-yocto-build)

Apparently the bug is known and is being patched: [source](https://bugzilla.opensuse.org/show_bug.cgi?id=981625)
