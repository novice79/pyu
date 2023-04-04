#!/bin/bash

set -e 
# brew uninstall aarch64-unknown-linux-musl armv7-unknown-linux-musleabihf x86_64-unknown-linux-musl
# brew untap messense/macos-cross-toolchains
# brew install armv7-ctng-linux-musleabihf aarch64-ctng-linux-musl x86_64-ctng-linux-musl
PRBUILT="$HOME/clib-prebuilt/linux/aarch64"
examplePrefix="$PWD/dist/linux/aarch64"
PREFIX=${prefix:-$examplePrefix}
dir="$PWD/_build/linux/aarch64"

while [ $# -gt 0 ]; do
  case "$1" in
    -prefix=* | --PREFIX=* | prefix=* | PREFIX=*)
      PREFIX="${1#*=}"
      ;;
    *)
      printf "remove $dir for clean build\n"
      rm -rf $dir
  esac
  shift
done
PREFIX="${PREFIX%/aarch64}/aarch64"

cmake -H"src" -B"$dir/lib" \
-G Ninja \
-DCMAKE_TOOLCHAIN_FILE="$(brew --prefix aarch64-ctng-linux-musl)/toolchain.cmake" \
-DTARGET_TRIPLET="aarch64-ctng-linux-musl" \
-DCMAKE_FIND_ROOT_PATH=$PRBUILT \
-DCMAKE_BUILD_TYPE=Release \
-DCMAKE_INSTALL_PREFIX=$PREFIX


cmake --build "$dir/lib" --config Release
cmake --install $dir/lib

# build example exe
cmake -GNinja -H"examples" -B$dir \
-DCMAKE_TOOLCHAIN_FILE="$(brew --prefix aarch64-ctng-linux-musl)/toolchain.cmake" \
-DCMAKE_FIND_ROOT_PATH="$PREFIX;$PRBUILT" \
-DCMAKE_INSTALL_PREFIX=$examplePrefix \
-DCMAKE_BUILD_TYPE=Release 

cmake --build $dir
cmake --install $dir

# strip $examplePrefix/app
ls -lh $examplePrefix/bin
file $examplePrefix/bin/app