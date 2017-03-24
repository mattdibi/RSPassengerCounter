# Yocto build guide

## Clone needed repositories
```sh
$ git clone git://git.yoctoproject.org/poky
$ cd ~/poky
$ git clone git://git.yoctoproject.org/meta-intel
$ git clone git://git.openembedded.org/meta-openembedded
$ git clone https://github.com/IntelRealSense/meta-intel-realsense.git
```

**Important**: Set them to track morty branch

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
```

## Modify conf files
* First you need to modify **local.conf**. Use the one provided in this repository
* Then add the **auto.conf** file needed for the librealsense library. Use the one provided in this repository

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
