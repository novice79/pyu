cmake_minimum_required(VERSION 3.21)

project(test)

set(CMAKE_C_STANDARD 17)
# set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 20)
# use all static lib
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") 
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden")
endif()
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -funwind-tables -fexceptions -frtti -lunwind")
add_compile_options(-fvisibility=hidden -fexceptions -frtti)
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -l/Users/piao/clib-prebuilt/android/arm64-v8a/lib/libunwind.a")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -l/Users/piao/clib-prebuilt/android/arm64-v8a/lib/libunwind.a")

# set(Boost_DEBUG ON)
set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
# set(Boost_USE_STATIC_RUNTIME    OFF)

find_package(Boost REQUIRED COMPONENTS 
    date_time 
    filesystem 
    serialization
    atomic 
    json
    log_setup 
    log 
    program_options
    system 
    # locale 
)
# find_package(Boost 1.77.0 REQUIRED COMPONENTS ALL)
# find_package (Boost 1.79.0 REQUIRED COMPONENTS ALL PATHS "$ENV{HOME}/boost" NO_DEFAULT_PATH)
# include_directories(${Boost_INCLUDE_DIRS})
# target_link_libraries(${name}
#     Boost::date_time 
#     Boost::filesystem 
#     Boost::regex 
#     Boost::atomic 
#     Boost::chrono 
#     Boost::log_setup 
#     Boost::thread 
#     Boost::log 
#     Boost::system 
#     Boost::locale
# )
# https://cmake.org/cmake/help/latest/module/FindBoost.html
# message("Boost_INCLUDE_DIRS=${Boost_INCLUDE_DIRS}")
# message("Boost_LIBRARIES=${Boost_LIBRARIES}")
# set(OPENSSL_USE_STATIC_LIBS TRUE)
message("Boost_LIBRARY_DIRS=${Boost_LIBRARY_DIRS}")
link_directories(${Boost_LIBRARY_DIRS})
# Make sure that your call to link_directories takes place before your call to the relevant add_executable
file(GLOB SOURCE_FILES  "*.cpp")
foreach(file ${SOURCE_FILES})
    get_filename_component(name "${file}" NAME_WE)
    add_executable(${name} "${file}" "android/miss_unwind.c")
    list(APPEND targets ${name})
    if(${CMAKE_SYSTEM_NAME} MATCHES "iOS")
        set_target_properties(${name} PROPERTIES
            BUNDLE True
            MACOSX_BUNDLE_GUI_IDENTIFIER piaoyun.dev.${name}
            MACOSX_BUNDLE_BUNDLE_NAME ${name}
            MACOSX_BUNDLE_BUNDLE_VERSION "0.1"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1"
            MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/../misc/customtemplate.plist.in
        )
        # This little macro lets you set any XCode specific property
        macro (set_xcode_property TARGET XCODE_PROPERTY XCODE_VALUE)
            set_property (TARGET ${TARGET} PROPERTY XCODE_ATTRIBUTE_${XCODE_PROPERTY} ${XCODE_VALUE})
        endmacro (set_xcode_property)
        macro (unset_xcode_property TARGET XCODE_PROPERTY)
        set_property (TARGET ${TARGET} PROPERTY XCODE_ATTRIBUTE_${XCODE_PROPERTY})
        endmacro (unset_xcode_property)
        set_xcode_property(${name} CODE_SIGN_IDENTITY "iPhone Developer")
        # grep DEVELOPMENT_TEAM _build_ios/main.xcodeproj/project.pbxproj
        set_xcode_property(${name} DEVELOPMENT_TEAM W8L2TKP6RD)
    endif()
endforeach()

find_package(OpenSSL 1.1.1 REQUIRED)
message("OPENSSL_CRYPTO_LIBRARY=${OPENSSL_CRYPTO_LIBRARY}")
message("OPENSSL_LIBRARIES=${OPENSSL_LIBRARIES}")


# sudo apt-get install libz-dev
find_package(ZLIB REQUIRED)
message("ZLIB_LIBRARIES=${ZLIB_LIBRARIES}")

# only for windows
# target_link_libraries(${name} wsock32)
find_package(pyu 1.0.0 REQUIRED)
get_target_property(pyu_INCLUDE_DIRS pyu::pyu INTERFACE_INCLUDE_DIRECTORIES )
message("pyu_INCLUDE_DIRS=${pyu_INCLUDE_DIRS}")
get_target_property(pyu_LIBRARIES pyu::pyu IMPORTED_LOCATION_RELEASE )
message("pyu_LIBRARIES=${pyu_LIBRARIES}")
message("pyu_VERSION=${pyu_VERSION}")
include_directories(${pyu_INCLUDE_DIRS})
# set(STATIC_LINK " -s -static  -static-libstdc++ -static-libgcc")
# if (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux")
#     # set_property(TARGET ${name} APPEND_STRING PROPERTY LINK_FLAGS " ${STATIC_LINK}")
#     string(APPEND CMAKE_EXE_LINKER_FLAGS "${STATIC_LINK}")
# endif ()
# if (ANDROID)
#     message(STATUS "Hello from Android build!")
#     find_library(log-lib log)
# endif()

find_library(lib7z NAMES 7za REQUIRED)
message("lib7z=${lib7z}")
# add_library(7z STATIC IMPORTED)
# set_target_properties(7z 
#     PROPERTIES IMPORTED_LOCATION 
#     ${lib7z}
# )

foreach(t ${targets})
    if (WIN32)
        set_target_properties(${t} PROPERTIES
            LINK_FLAGS "/WHOLEARCHIVE"
        )
    elseif (APPLE)
        set_target_properties(${t} PROPERTIES
            LINK_FLAGS "-Wl,-all_load"
        )
        target_link_libraries(${t} ${lib7z})
        # target_link_libraries(${t} -Wl,-force_load 7z)
    else ()
        # target_link_libraries(${PROJECT_NAME} -Wl,--whole-archive 7za -Wl,--no-whole-archive )
        target_link_libraries(${t} -Wl,--whole-archive ${lib7z} -Wl,--no-whole-archive )
    endif ()
    target_link_libraries(
        ${t}
        ${Boost_LIBRARIES}
        ${OPENSSL_LIBRARIES}
        ${ZLIB_LIBRARIES}
        # ZLIB::ZLIB
        pyu::pyu
        ${CMAKE_DL_LIBS} 
    )
    # if (ANDROID)
    #     target_link_libraries(${t} ${log-lib})
    # endif()
    if(WIN32)
        target_link_libraries(${t} ws2_32 wsock32)
    endif()
    if(${CMAKE_SYSTEM_NAME} MATCHES "iOS")
        install(TARGETS ${t} BUNDLE DESTINATION bin)
    else()
        install(TARGETS ${t} RUNTIME DESTINATION bin)
    endif()
    
    
endforeach()

# otool -L  dist/app


