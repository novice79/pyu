#! /bin/sh
# SDK="iphoneos"
# PREFIX=$HOME/clib-prebuilt/ios 
# MIN_IOS_VERSION=13.0
# OPT_FLAGS="-O3 -g3 -fembed-bitcode"
# PLATFORM_ARM=${PLATFORM}
# ARCH_FLAGS="-arch arm64 -arch arm64e" 
# HOST_FLAGS="${ARCH_FLAGS} -miphoneos-version-min=${MIN_IOS_VERSION} -isysroot $(xcrun --sdk ${SDK} --show-sdk-path)"
# CHOST="arm-apple-darwin"

# export CC=$(xcrun --find --sdk "${SDK}" clang)
# export CXX=$(xcrun --find --sdk "${SDK}" clang++)
# export AR=$(xcrun --find --sdk iphoneos ar)
# export objdump=$(xcrun --find --sdk iphoneos objdump)
# export CPP=$(xcrun --find --sdk "${SDK}" cpp)
# export CFLAGS="${HOST_FLAGS} ${OPT_FLAGS} -I${PREFIX}/include"
# export CXXFLAGS="${HOST_FLAGS} ${OPT_FLAGS} -I${PREFIX}/include"
# export LDFLAGS="${HOST_FLAGS} -L${PREFIX}/lib "
set -e
scirptName=$0
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        PRBUILT="/data/clib-prebuilt/ios"
elif [[ "$OSTYPE" == "darwin"* ]]; then
        PRBUILT="$HOME/clib-prebuilt/ios"
elif [[ "$OSTYPE" == "win32" ]]; then
    :
else
    :
fi
dir="_build/ios"
examplePrefix="dist/ios"
PREFIX=${prefix:-$examplePrefix}
while [ $# -gt 0 ]; do
  case "$1" in
    -prefix=* | --PREFIX=* | prefix=* | PREFIX=*)
      PREFIX="${1#*=}"
      ;;
    -h | --help)
      echo "build example app(s) and install lib to target dir with:"
      echo "$scirptName prefix=\$targetDir"
      echo "default lib installation dir: $examplePrefix --if without prefix provided"
      echo "example app always install to $examplePrefix "
      exit 0
      ;;
    *)
      printf "remove $dir for clean build\n"
      rm -rf $dir
  esac
  shift
done
# CMAKE_PREFIX_PATH vs CMAKE_FIND_ROOT_PATH
cmake -H"src" -B"$dir/lib" \
-G Xcode \
-DCMAKE_TOOLCHAIN_FILE="$PWD/cmake/ios.toolchain.cmake" \
-DDEPLOYMENT_TARGET=13.0 \
-DPLATFORM=OS64 \
-DCMAKE_FIND_ROOT_PATH="$PRBUILT" \
-DCMAKE_BUILD_TYPE=Release \
-DCMAKE_INSTALL_PREFIX=$PREFIX

cmake --build "$dir/lib" --config Release -- -allowProvisioningUpdates
cmake --install "$dir/lib"


cmake -H"examples" -B"$dir" \
-G Xcode \
-DCMAKE_TOOLCHAIN_FILE="$PWD/cmake/ios.toolchain.cmake" \
-DDEPLOYMENT_TARGET=13.0 \
-DPLATFORM=OS64 \
-DCMAKE_FIND_ROOT_PATH="${PREFIX};${PRBUILT}" \
-DCMAKE_BUILD_TYPE=Release \
-DCMAKE_INSTALL_PREFIX=$examplePrefix
# grep DEVELOPMENT_TEAM _build_ios/main.xcodeproj/project.pbxproj
cmake --build $dir --config Release -- -allowProvisioningUpdates
cmake --install $dir