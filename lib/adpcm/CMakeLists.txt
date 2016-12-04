cmake_minimum_required(VERSION 3.1)
project(adpcm)

add_library(adpcm STATIC adpcm-lib.cpp)
set_target_properties(adpcm PROPERTIES LINKER_LANGUAGE C)

add_executable(decode "decode.cpp")
target_link_libraries(decode adpcm)
