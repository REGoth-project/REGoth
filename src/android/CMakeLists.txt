cmake_minimum_required(VERSION 2.8.3)

# Usage:
# cmake -DANDROID_NDK=$NDK -DCMAKE_TOOLCHAIN_FILE=$NDK/cmake/toolchain.cmake -DANDROID_ABI=armeabi-v7a -DANDROID_NATIVE_API_LEVEL=23 ..


project(testBuilder)
     
include("Apk.cmake" REQUIRED)
     
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(ARM_TARGET armeabi-v7a)
     
include_directories(${ANDROID_NDK}/sources/android/native_app_glue)
     
set(TEST_SRC
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
    ../target/hydravis.cpp
)
     
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -ffor-scope -fno-rtti -fno-exceptions -pipe -ffunction-sections -fdata-sections -ffast-math -Wnon-virtual-dtor -Wreorder -Wsign-promo -fvisibility=hidden -fvisibility-inlines-hidden -Wstrict-null-sentinel -Os -funroll-all-loops -fpeel-loops -ftree-vectorize")
set(LINKER_FLAGS "${LINKER_FLAGS} -Wl,--as-needed -Wl,--gc-sections -Wl,--no-undefined -Wl,--strip-all -Wl,-rpath-link=${ANDROID_NDK_SYSROOT}/usr/lib/ -L${ANDROID_NDK_SYSROOT}/usr/lib/")
     
add_library(testapk SHARED ${TEST_SRC})
     
target_link_libraries(testapk log android crystax)
set_target_properties(testapk PROPERTIES COMPILE_DEFINITIONS "ANDROID")
     
set(APP_SHARED_LIBRARIES "${LIBRARY_OUTPUT_PATH}/libtestapk.so")
#set(APP_SHARED_LIBRARIES "${APP_SHARED_LIBRARIES};${CMAKE_CURRENT_SOURCE_DIR}/lib/armeabi-v7a/libcrystax.so")
set(APP_SHARED_LIBRARIES "${APP_SHARED_LIBRARIES};${CMAKE_CURRENT_SOURCE_DIR}/lib/x86/libcrystax.so")
     
message(STATUS ${APP_SHARED_LIBRARIES})
     
android_create_apk(testapk "${CMAKE_BINARY_DIR}/apk" "${APP_SHARED_LIBRARIES}" "" "Data")
 
