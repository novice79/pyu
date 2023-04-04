#!/usr/bin/env bash

# build with android-ndk-r25c
# set -x

if [[ $OSTYPE == "darwin"* ]]; then
  ncpu=$(sysctl -n hw.physicalcpu)
  os_type=darwin
else
  ncpu=$(nproc)
  os_type=linux
fi
tc_path="$NDK/toolchains/llvm/prebuilt/${os_type}-x86_64/bin"
target=armv7a-linux-androideabi
API=23

export AR=$tc_path/llvm-ar
export AS=$tc_path/llvm-as
export STRIP=$tc_path/llvm-strip
export RANLIB=$tc_path/llvm-ranlib
export objdump=$tc_path/llvm-objdump
export CC=$tc_path/$target$API-clang
export CXX=$tc_path/$target$API-clang++








