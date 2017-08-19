#!/bin/bash
basepath=$(cd `dirname $0`; pwd)
export TMPDIR=$basepath/ffmpegtmp

NDK=/Users/xc/installer/android-ndk-r14b
SYSROOT=$NDK/platforms/android-23/arch-x86/
TOOLCHAIN=$NDK/toolchains/x86-4.9/prebuilt/darwin-x86_64

CPU=x86
PREFIX=$basepath/android/x86/

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
  --cross-prefix=$TOOLCHAIN/bin/i686-linux-android- \
  --target-os=linux \
  --arch=x86 \
  --enable-cross-compile \
  --sysroot=$SYSROOT \
  --extra-cflags="-Os -fpic" \
$ADDITIONAL_CONFIGURE_FLAG
make clean
make -j4
make install
}

build_one
