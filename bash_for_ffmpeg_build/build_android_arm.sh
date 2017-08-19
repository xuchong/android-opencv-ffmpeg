#!/bin/bash
basepath=$(cd `dirname $0`; pwd)
export TMPDIR=$basepath/ffmpegtmp

NDK=/Users/xc/installer/android-ndk-r14b
SYSROOT=$NDK/platforms/android-23/arch-arm/
TOOLCHAIN=$NDK/toolchains/arm-linux-androideabi-4.9/prebuilt/darwin-x86_64

CPU=arm
PREFIX=$basepath/android/arm/
ADDI_CFLAGS="-marm"

function build_one
{
./configure \
  --prefix=$PREFIX \
  --enable-shared \
  --disable-static \
  --disable-doc \
  --disable-ffmpeg \
  --disable-ffplay \
  --disable-ffprobe \
  --disable-ffserver \
  --disable-doc \
  --disable-symver \
  --enable-small \
  --disable-yasm \
  --cross-prefix=$TOOLCHAIN/bin/arm-linux-androideabi- \
  --target-os=linux \
  --arch=arm \
  --enable-cross-compile \
  --sysroot=$SYSROOT \
  --extra-cflags="-Os -fpic $ADDI_CFLAGS" \
$ADDITIONAL_CONFIGURE_FLAG
make clean
make -j4
make install
}

build_one
