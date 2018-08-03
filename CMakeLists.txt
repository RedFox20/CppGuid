cmake_minimum_required(VERSION 3.8)
project(CppGuid)

include(mama.cmake)
include_directories(${MAMA_INCLUDES})


if(CLANG OR GCC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")
    if (CLANG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-shorten-64-to-32")
    endif()
    if (LINUX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
    elseif (APPLE) # enable Objective-C++
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fobjc-abi-version=2 -fobjc-arc")
    endif()
elseif(MSVC)
    # /MD - MultiThreaded Dynamic CRT
    # /wd4996 - Disable deprecation warning
    # /wd4819 - Disable code page warning (OpenCV)
    # /wd4251 - Disable dll-interface warning
    # /wd4275 - Disable dll-interface base class warning
    # /W2 - warning level 2
    # /Zi - Debug Database
    # /Oi - Intrinsics Enabled
    # /MP - MultiProcess build
    # /arch:AVX2 
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest /Oi /wd4996 /wd4819 /wd4251 /wd4275 /W2 /Zi /MP")
    add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)
    
    foreach(MODE "_DEBUG" "_MINSIZEREL" "_RELEASE" "_RELWITHDEBINFO")
        string(REPLACE "/MDd" "/MD" TMP "${CMAKE_C_FLAGS${MODE}}")
        set(CMAKE_C_FLAGS${MODE} "${TMP}" CACHE STRING "" FORCE)
        message(STATUS "C_${MODE}=${CMAKE_C_FLAGS${MODE}}")
        string(REPLACE "/MDd" "/MD" TMP "${CMAKE_CXX_FLAGS${MODE}}")
        set(CMAKE_CXX_FLAGS${MODE} "${TMP}" CACHE STRING "" FORCE)
        message(STATUS "CXX_${MODE}=${CMAKE_CXX_FLAGS${MODE}}")
    endforeach(MODE)
endif()


option(CPPGUID_BUILD_TESTS "Build CppGuid test suite" OFF)


list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
if(LINUX)
    find_package(UUID REQUIRED)
    set(LIBS ${UUID_LIBRARY})
    message("Linux LIBS: ${LIBS}")
endif()


include_directories("include")
file(GLOB_RECURSE CPPGUID_PUBLIC_INTERFACE include/*.h)
file(GLOB_RECURSE CPPGUID_PRIVATE_SOURCES src/*.c src/*.cpp src/*.h)
source_group(include FILES ${CPPGUID_PUBLIC_INTERFACE})
source_group(src FILES ${CPPGUID_PRIVATE_SOURCES})

add_library(CppGuid STATIC ${CPPGUID_PUBLIC_INTERFACE} ${CPPGUID_PRIVATE_SOURCES})
target_link_libraries(CppGuid PUBLIC ${LIBS})
install(TARGETS CppGuid DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/lib)


if (CPPGUID_BUILD_TESTS AND (LINUX OR WIN32 OR MACOS))
    message(STATUS "Enabled CppGuidTests")
    file(GLOB_RECURSE CPPGUID_TESTS tests/*.c tests/*.cpp tests/*.h)
    source_group(tests FILES ${CPPGUID_TESTS})

    add_executable(CppGuidTests ${CPPGUID_TESTS})
    target_link_libraries(CppGuidTests PRIVATE CppGuid)
    install(TARGETS CppGuidTests DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/lib)
endif()
