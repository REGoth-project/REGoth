
if(ANDROID)
    add_library(REGoth SHARED "REGoth.cpp")
    target_link_libraries(REGoth engine)

    set(APP_SHARED_LIBRARIES "${APP_SHARED_LIBRARIES};${LIBRARY_OUTPUT_PATH}/lib/libREGoth.so")

    android_create_apk(REGoth "${CMAKE_BINARY_DIR}/apk" "${APP_SHARED_LIBRARIES}" "" "Data")
else()
    if(EMSCRIPTEN)
        set(CMAKE_EXECUTABLE_SUFFIX ".html")
    endif()
    add_executable(REGoth "REGoth.cpp")
    target_link_libraries(REGoth engine)
endif()

if(REGOTH_NETPLAY)
    add_executable(REGoth-server "REGoth-server.cpp")
    target_link_libraries(REGoth-server engine)
endif()
