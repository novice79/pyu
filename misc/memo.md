gh repo create pyu --public
git remote add origin n79:novice79/pyu
<!-- git remote set-url origin n79:novice79/pyu -->
sudo apt install p7zip-full
<!-- 
export HOMEBREW_NO_ANALYTICS=1
brew update
brew upgrade
brew info gcc
brew install gcc
brew cleanup 
-->

lldb --file ./dist/bin/app

<!-- todo: rebuild all android lib with android-ndk-r25c -->
<!-- create empty branch -->
git switch --orphan file
git commit --allow-empty -m "Initial commit on orphan branch"
git push -u origin <new branch>

lipo -create "${ARCH_FILES[@]}" -o "universal/libboost_$NAME.a"
lipo libMy_armv6.a libMy_armv7.a libMy_i368.a -create -output libMy.a
<!-- lipo can only use in mac ? -->
lipo -create \
/Users/piao/git/7z/dist/android/arm64-v8a/lib/lib7za.a \
dist/android/arm64-v8a/lib/libpyu.a \
-o dist/android/arm64-v8a/lib/libpyu.a_

mv dist/android/arm64-v8a/lib/libpyu.a{_,}

lipo -create \
/Users/piao/git/7z/dist/macos/lib/lib7za.a \
dist/darwin20/lib/libpyu.a \
-o dist/darwin20/lib/libpyu.a_

mv dist/darwin20/lib/libpyu.a{_,}


lib=$(
cat << EOF
create dist/darwin20/lib/libpyu.a_
addlib /Users/piao/git/7z/dist/macos/lib/lib7za.a
addlib dist/darwin20/lib/libpyu.a
save
end
EOF

)
ar -M $lib

libtool -static -o dist/darwin20/lib/libpyu.a_ \
/Users/piao/git/7z/dist/macos/lib/lib7za.a \
dist/darwin20/lib/libpyu.a

${CMAKE_SYSTEM_PROCESSOR}-novice-${CMAKE_SYSTEM_NAME}

zig c++ -target armeb-linux-musleabihf

rsync -aP pyu ub220:/data/my-app/