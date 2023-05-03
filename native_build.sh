#!/bin/sh
set -e

scirptName=$0

ostype=$(uname)
if [[ "$ostype" == "Linux"* ]]; then
    PRBUILT="/data/clib-prebuilt/linux/x86_64"
elif [[ "$OSTYPE" == "Darwin"* ]]; then
    PRBUILT="$HOME/clib-prebuilt/macos"
else
    :
fi
dir="$PWD/_build"
examplePrefix="$PWD/dist"
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
        lib)
            LIB_ONLY=true
        ;;
        *)
            printf "remove $dir for clean build\n"
            rm -rf $dir
    esac
    shift
done

# build http lib
cmake -GNinja -H"src" -B"$dir/lib" \
-DCMAKE_FIND_ROOT_PATH="$PRBUILT" \
-DCMAKE_INSTALL_PREFIX=$PREFIX \
-DCMAKE_BUILD_TYPE=Release

cmake --build "$dir/lib"
cmake --install "$dir/lib"
[ "$LIB_ONLY" = true ] && exit 0
# build example exe
cmake -GNinja -H"examples" -B$dir \
-DCMAKE_FIND_ROOT_PATH="$PREFIX;$PRBUILT" \
-DCMAKE_INSTALL_PREFIX=$examplePrefix \
-DCMAKE_BUILD_TYPE=Release

cmake --build $dir
cmake --install $dir

