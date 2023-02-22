#!/bin/sh
# set -x

scirptName=$0

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        PRBUILT="/data/clib-prebuilt/linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
        PRBUILT="$HOME/clib-prebuilt/macos"
elif [[ "$OSTYPE" == "win32" ]]; then
    :
else
    :
fi
dir="_build/$OSTYPE"
examplePrefix="dist/$OSTYPE"
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

# build http lib
cmake -GNinja -H"src" -B"$dir/lib" \
-DCMAKE_FIND_ROOT_PATH="$PRBUILT" \
-DCMAKE_INSTALL_PREFIX=$PREFIX \
-DCMAKE_BUILD_TYPE=Release 

cmake --build "$dir/lib"
cmake --install "$dir/lib"

# build example exe
cmake -GNinja -H"examples" -B$dir \
-DCMAKE_FIND_ROOT_PATH="$PREFIX;$PRBUILT" \
-DCMAKE_INSTALL_PREFIX=$examplePrefix \
-DCMAKE_BUILD_TYPE=Release 

cmake --build $dir
cmake --install $dir

