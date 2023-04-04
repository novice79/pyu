#! /bin/sh
SDK="iphoneos"
PREFIX=$HOME/clib-prebuilt/ios 
MIN_IOS_VERSION=13.0
OPT_FLAGS="-O3 -g3 -fembed-bitcode"
PLATFORM_ARM=${PLATFORM}
ARCH_FLAGS="-arch arm64" 
HOST_FLAGS="${ARCH_FLAGS} -miphoneos-version-min=${MIN_IOS_VERSION} -isysroot $(xcrun --sdk ${SDK} --show-sdk-path)"
CHOST="arm-apple-darwin"
# xcrun --find --sdk iphoneos clang
export CC=$(xcrun --find --sdk "${SDK}" clang)
export CXX=$(xcrun --find --sdk "${SDK}" clang++)
export AR=$(xcrun --find --sdk iphoneos ar)
export objdump=$(xcrun --find --sdk iphoneos objdump)
export CPP=$(xcrun --find --sdk "${SDK}" cpp)
export CFLAGS="${HOST_FLAGS} ${OPT_FLAGS} -I${PREFIX}/include"
export CXXFLAGS="${HOST_FLAGS} ${OPT_FLAGS} -I${PREFIX}/include"
export LDFLAGS="${HOST_FLAGS} -L${PREFIX}/lib "


# ./configure \
#     --host="${CHOST}" \
#     --prefix="${PREFIX}" \
#     --enable-static \
#     --disable-shared 

# make -j`nproc` clean install