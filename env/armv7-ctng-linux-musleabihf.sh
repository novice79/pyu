#! /bin/sh

COMPILER_PREFIX=armv7-ctng-linux-musleabihf
export CC=${COMPILER_PREFIX}-gcc
export CXX=${COMPILER_PREFIX}-g++
export AR=${COMPILER_PREFIX}-ar
export RANLIB=${COMPILER_PREFIX}-ranlib
export STRIP=${COMPILER_PREFIX}-strip


