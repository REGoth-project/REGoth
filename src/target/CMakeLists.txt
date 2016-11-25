
if(ANDROID)
    add_library(REGoth SHARED "REGoth.cpp")
    target_link_libraries(REGoth engine)

    set(APP_SHARED_LIBRARIES "${APP_SHARED_LIBRARIES};${LIBRARY_OUTPUT_PATH}/lib/libREGoth.so")

    android_create_apk(REGoth "${CMAKE_BINARY_DIR}/apk" "${APP_SHARED_LIBRARIES}" "" "Data")
else()
    add_executable(REGoth "REGoth.cpp")
    target_link_libraries(REGoth engine)
endif()