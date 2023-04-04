#! /bin/sh

COMPILER_PREFIX=aarch64-ctng-linux-musl
export CC=${COMPILER_PREFIX}-gcc
export CXX=${COMPILER_PREFIX}-g++
export AR=${COMPILER_PREFIX}-ar
export RANLIB=${COMPILER_PREFIX}-ranlib
export STRIP=${COMPILER_PREFIX}-strip


