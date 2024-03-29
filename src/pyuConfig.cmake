include(CMakeFindDependencyMacro)
set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_dependency(Boost 1.79.0 REQUIRED
    filesystem 
    # serialization
    json
    log_setup 
    log 
    program_options
    system 
)
find_dependency(OpenSSL 1.1.1 REQUIRED)
find_dependency(ZLIB REQUIRED)
include(${CMAKE_CURRENT_LIST_DIR}/pyu.cmake)