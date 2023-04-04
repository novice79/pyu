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
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -fexceptions -frtti ")
# add_compile_options(-fvisibility=hidden -fexceptions -frtti)
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

# only for windows
# target_link_libraries(${name} wsock32)
find_package(pyu 1.0.0 REQUIRED)
get_target_property(pyu_INCLUDE_DIRS pyu::pyu INTERFACE_INCLUDE_DIRECTORIES )
message("pyu_INCLUDE_DIRS=${pyu_INCLUDE_DIRS}")
get_target_property(pyu_LIBRARIES pyu::pyu IMPORTED_LOCATION_RELEASE )
message("pyu_LIBRARIES=${pyu_LIBRARIES}")
message("pyu_VERSION=${pyu_VERSION}")
include_directories(${pyu_INCLUDE_DIRS})

find_library(lib7z NAMES 7za REQUIRED)
message("lib7z=${lib7z}")
# add_library(7z STATIC IMPORTED)
# set_target_properties(7z 
#     PROPERTIES IMPORTED_LOCATION 
#     ${lib7z}
# )
message("CMAKE_CXX_COMPILER_ID=${CMAKE_CXX_COMPILER_ID}")
foreach(t ${targets})
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        # using Clang
        target_link_libraries(${t} -Wl,-all_load )
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # using GCC
        target_link_libraries(${t} -Wl,--whole-archive ${lib7z} -Wl,--no-whole-archive )
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
        # using Intel C++
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        # using Visual Studio C++
        set_target_properties(${t} PROPERTIES LINK_FLAGS "/WHOLEARCHIVE" )
    endif()
    # if (WIN32)
        
    # elseif (APPLE)
    #     set_target_properties(${t} PROPERTIES
    #         LINK_FLAGS "-Wl,-all_load"
    #     )
    #     target_link_libraries(${t} ${lib7z})
    #     # target_link_libraries(${t} -Wl,-force_load,${lib7z})
    # else ()
    #     # target_link_libraries(${PROJECT_NAME} -Wl,--whole-archive 7za -Wl,--no-whole-archive )
    #     target_link_libraries(${t} -Wl,--whole-archive ${lib7z} -Wl,--no-whole-archive )
    # endif ()
    target_link_libraries(
        ${t}
        pyu::pyu
        ${CMAKE_DL_LIBS} 
    )
    if(WIN32)
        target_link_libraries(${t} ws2_32 wsock32)
    endif()
    if(${CMAKE_SYSTEM_NAME} MATCHES "iOS")
        install(TARGETS ${t} BUNDLE DESTINATION bin)
    else()
        install(TARGETS ${t} RUNTIME DESTINATION bin)
    endif()
    
endforeach()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../misc/magic.mgc" DESTINATION bin)
# otool -L  dist/app


