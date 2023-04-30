cmake_minimum_required(VERSION 3.21)

project(pyu LANGUAGES C CXX VERSION 1.0.0)
if (MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
add_subdirectory(7z)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -fexceptions -frtti")
file(GLOB_RECURSE SOURCE_FILES 
    "src/*.cpp"
    "qrgen/*.cpp"
    "log/*.cpp"
    "lua/*.c"
    "sqlite/*.c"
    "db/*.cpp"
)
file(GLOB HEADER_FILES 
    "include/pyu.h" 
    "file/src/magic.h"
    "file/file.h"
    "7z/p7z.h"
    "qrgen/qrcodegen.hpp"
    "qrgen/QrToPng.h"
    "log/log.h"
    "lua/lua.h"
    "lua/lualib.h"
    "lua/lauxlib.h"
    "lua/luaconf.h"
    "lua/lua.hpp"
    "sqlite/sqlite3.h"
    "db/db.h"
)
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") 
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden")
endif()
include(unrar/files.cmake)
# include(7z/files.cmake)

add_library(${PROJECT_NAME} STATIC 
    ${SOURCE_FILES} 
    ${UNRAR_FILES}
    # ${7z_FILES}
)
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${HEADER_FILES}")
include_directories(
    "${CMAKE_SOURCE_DIR}/inc"
    "${CMAKE_SOURCE_DIR}/sqlite"
    "${CMAKE_SOURCE_DIR}/file/src"
)
set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

find_package(Boost 1.79.0 REQUIRED COMPONENTS 
    filesystem 
    # serialization
    json
    log_setup 
    log 
    program_options
    system 
)
# include_directories(${Boost_INCLUDE_DIRS})
target_include_directories(
    ${PROJECT_NAME} 
    PUBLIC
    ${Boost_INCLUDE_DIRS}
)

find_package(OpenSSL 1.1.1 REQUIRED)
find_package(ZLIB REQUIRED)
target_link_libraries(
    ${PROJECT_NAME} 
    PUBLIC
    ${Boost_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    ${ZLIB_LIBRARIES}
    # 7za
)
if(ANDROID_ABI STREQUAL "armeabi-v7a")
    set(FILE_HOST armv7a-linux-androideabi)
elseif (ANDROID_ABI STREQUAL "arm64-v8a")
    set(FILE_HOST aarch64-linux-android)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "iOS")
    set(FILE_HOST arm-apple-darwin)
else()
    set(FILE_HOST ${TARGET_TRIPLET})
endif ()

add_custom_command(TARGET ${PROJECT_NAME}
POST_BUILD
# COMMAND autoreconf -ivf ${CMAKE_CURRENT_SOURCE_DIR}/file
COMMAND sh -c "\
[[ -f ${CMAKE_CURRENT_SOURCE_DIR}/../env/${FILE_HOST}.sh ]] \
&& source ${CMAKE_CURRENT_SOURCE_DIR}/../env/${FILE_HOST}.sh; \
${CMAKE_CURRENT_SOURCE_DIR}/file/configure \
--host=${FILE_HOST} \
--prefix=${CMAKE_INSTALL_PREFIX} \
--disable-shared --enable-static \
--disable-zstdlib \
--disable-bzlib \
--disable-xzlib; \
make clean; \
make -j4; \
pyu=`find . -name libpyu.a | tail -n 1`; \
mv -v $pyu libpyu_.a; \
rm -f *.o; \
${CMAKE_AR} -x libpyu_.a; \
${CMAKE_AR} -x src/.libs/libmagic.a; \
${CMAKE_AR} -qcs $pyu *.o; \
rm -f *.o"
VERBATIM
# COMMAND rm -f ${CMAKE_INSTALL_PREFIX}/lib/libmagic.*
WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
COMMENT "Build libmagic and merge to pyu lib"
)
# objdum -f src/.libs/libmagic.a
if (WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS "/WHOLEARCHIVE"
    )
elseif (APPLE)
    # set_target_properties(${PROJECT_NAME} PROPERTIES
    #     LINK_FLAGS "-Wl,-all_load"
    # )
    # target_link_libraries(${PROJECT_NAME} PUBLIC 7za)
    target_link_libraries(${PROJECT_NAME} PUBLIC -Wl,-force_load 7za)
else ()
    target_link_libraries(${PROJECT_NAME} PUBLIC -Wl,--whole-archive 7za -Wl,--no-whole-archive )
endif ()

install(DIRECTORY sol DESTINATION "include/pyu")
install(
    TARGETS ${PROJECT_NAME} 7za
    EXPORT pyu
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
    PUBLIC_HEADER DESTINATION "include/pyu"
)
install(EXPORT pyu
    FILE pyu.cmake
    NAMESPACE pyu::
    DESTINATION lib/cmake/pyu
)
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "pyuConfigVersion.cmake"
    VERSION ${pyu_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES 
    "pyuConfig.cmake" 
    "${CMAKE_CURRENT_BINARY_DIR}/pyuConfigVersion.cmake"
    DESTINATION lib/cmake/pyu
)

