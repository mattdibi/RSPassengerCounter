# Yocto build guide

## Clone needed repositories
```sh
$ git clone git://git.yoctoproject.org/poky
$ cd ~/poky
$ git clone git://git.yoctoproject.org/meta-intel
$ git clone git://git.openembedded.org/meta-openembedded
$ git clone https://github.com/IntelRealSense/meta-intel-realsense.git
$ git clone git://git.yoctoproject.org/meta-oracle-java
```

**Important**: Set them to track morty branch

```sh
$ git checkout morty
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
$ bitbake-layers add-layer "$HOME/poky/meta-oracle-java"
```

## Modify conf files
* First you need to modify **local.conf**. Use the one provided in this repository
* Then add the **auto.conf** file needed for the librealsense library. Use the one provided in this repository

## Modify oracle-jse-jre recipe
It is then needed to add a dependency in **meta-oracle-java/recipes-devtools/oracle-java/oracle-jse-jre_1.8.0.bb** recipe.
The resulting file will be:

```bb
#Automatically choose java package based on target architecture

#Added line:
DEPENDS = "libxslt"

def get_java_pkg(d):
       TA = d.getVar('TARGET_ARCH', True)
       if TA == "arm":
               javaPkg = "oracle-jse-ejre-arm-vfp-hflt-client-headless"
       elif TA == "i586":
               javaPkg = "oracle-jse-jre-i586"
       elif TA == "x86_64":
               javaPkg = "oracle-jse-jre-x86-64"
       else:
               raise bb.parse.SkipPackage("Target architecture '%s' is not supported by the meta-oracle-java layer" %TA)

       return javaPkg

JAVA_PKG = "${@get_java_pkg(d)}"

require ${JAVA_PKG}.inc
```

## Modify rmc.bb recipe
It is needed to add some lines to **meta-intel/common/recipes-bsp/rmc/rmc.bb** to correctly install some lib files.
The resulting file will be:
```bb
...

do_install_class-native() {
        install -d ${D}${STAGING_BINDIR_NATIVE}
        install -m 0755 ${S}/src/rmc ${D}${STAGING_BINDIR_NATIVE}
}

FILES_${PN} += "/usr/lib"
FILES_${PN} += "/usr/lib/librsmpefi.a"
FILES_${PN} += "/usr/lib/librmclefi.a"

BBCLASSEXTEND = "native"
```

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
$ bitbake core-image-sato
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
