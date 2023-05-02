if(ANDROID_ABI STREQUAL "armeabi-v7a")
    set(FILE_HOST armv7a-linux-androideabi)
elseif (ANDROID_ABI STREQUAL "arm64-v8a")
    set(FILE_HOST aarch64-linux-android)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "iOS")
    set(FILE_HOST arm-apple-darwin)
else()
    set(FILE_HOST ${COMPILER_PREFIX})
endif ()
# if(FILE_HOST)
    set(CC ${CMAKE_C_COMPILER})
# endif ()
message("FILE_HOST=${FILE_HOST}")
message("CMAKE_C_FLAGS=${CMAKE_C_FLAGS}")
message("CMAKE_FIND_ROOT_PATH=${CMAKE_FIND_ROOT_PATH}")
foreach(dir ${CMAKE_FIND_ROOT_PATH})
#   message(STATUS "dir='${dir}'")
    set(LDFLAGS "${LDFLAGS} -L${dir}")
    set(CPPFLAGS "${CPPFLAGS} -I${dir}")
endforeach()
add_custom_target(magic
    COMMAND autoreconf -ivf ${CMAKE_CURRENT_SOURCE_DIR}/file
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/file/configure 
    CC=${CC}
    # CXX=${CMAKE_CXX_COMPILER}
    AR=${CMAKE_AR}
    STRIP=${CMAKE_STRIP}
    RANLIB=${CMAKE_RANLIB}
    CFLAGS=${CMAKE_C_FLAGS}
    # CXXFLAGS=${CMAKE_CXX_FLAGS}
    LDFLAGS=${LDFLAGS}
    CPPFLAGS=${CPPFLAGS}
    --host="${FILE_HOST}"
    --prefix=${CMAKE_INSTALL_PREFIX}
    --disable-shared --enable-static 
    --disable-zstdlib
    --disable-bzlib
    --disable-xzlib
    COMMAND ${MAKE} -j4 clean install
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
add_dependencies(${PROJECT_NAME} magic)
target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_INSTALL_PREFIX}/lib/libmagic.a)

# foreach(i ${Boost_LIBRARIES})
#     get_target_property(lib_i ${i} IMPORTED_LOCATION_RELEASE )
#     set(boost_libs "${boost_libs} ${lib_i}")
# endforeach()
# string(STRIP ${boost_libs} boost_libs)