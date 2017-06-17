
if(ANDROID)
    add_library(REGoth SHARED "REGoth.cpp")
    target_link_libraries(REGoth engine)
else()

    if(EMSCRIPTEN)
        set(CMAKE_EXECUTABLE_SUFFIX ".html")
    endif()
    add_executable(REGoth "REGoth.cpp")
    target_link_libraries(REGoth engine)
endif()