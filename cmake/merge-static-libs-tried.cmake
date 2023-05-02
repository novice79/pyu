if(ANDROID_ABI STREQUAL "armeabi-v7a")
    set(FILE_HOST armv7a-linux-androideabi)
elseif (ANDROID_ABI STREQUAL "arm64-v8a")
    set(FILE_HOST aarch64-linux-android)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "iOS")
    set(FILE_HOST arm-apple-darwin)
else()
    set(FILE_HOST ${COMPILER_PREFIX})
endif ()
if(FILE_HOST)
    set(CC ${CMAKE_C_COMPILER})
endif ()
message("CC=${CC}")
message("FILE_HOST=${FILE_HOST}")

add_custom_command(TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND autoreconf -ivf ${CMAKE_CURRENT_SOURCE_DIR}/file
COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/file/configure 
CC=${CC}
CFLAGS=${CMAKE_C_FLAGS}
LDFLAGS=${CMAKE_STATIC_LINKER_FLAGS}
CPPFLAGS=${CMAKE_C_COMPILE_FLAGS}
--host="${FILE_HOST}"
--prefix=${CMAKE_INSTALL_PREFIX}
--disable-shared --enable-static 
COMMAND make -j4 clean install
COMMAND mv ${CMAKE_INSTALL_PREFIX}/include/{magic.h,pyu/}
COMMAND rm -f *.o
COMMAND mv libpyu.a libpyu_.a
COMMAND ${CMAKE_AR} -x libpyu_.a
COMMAND ${CMAKE_AR} -x ${CMAKE_INSTALL_PREFIX}/lib/libmagic.a
COMMAND ${CMAKE_AR} -qcs libpyu.a *.o
COMMAND rm -f ${CMAKE_INSTALL_PREFIX}/lib/libmagic.*
WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
COMMENT "Build libmagic and merge to pyu lib"
)