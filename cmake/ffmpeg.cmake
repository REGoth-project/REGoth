include(ExternalProject)

function(FFMPEG_BUILD SOURCE_DIR BUILD_DIR)
    if (UNIX)
        set(BINARY_DIR "${BUILD_DIR}/build")
        set(INSTALL_DIR "${BUILD_DIR}/install")

        if(NOT EXISTS "${BINARY_DIR}")
            file(MAKE_DIRECTORY "${BINARY_DIR}")
        endif()

        ExternalProject_Add(FFMPEG_PROJECT
                SOURCE_DIR "${SOURCE_DIR}"
                CONFIGURE_COMMAND "${SOURCE_DIR}/configure" "--prefix=\"${INSTALL_DIR}\""
                    --disable-iconv
                    --disable-programs
                    --disable-doc
                    --disable-everything
                BINARY_DIR "${BINARY_DIR}"
                BUILD_COMMAND make
        )

        foreach(LIBRARY ${ARGN})
            add_library(FFMPEG_${LIBRARY} IMPORTED STATIC)
            set_property(TARGET FFMPEG_${LIBRARY}
                    PROPERTY IMPORTED_LOCATION "${INSTALL_DIR}/lib/lib${LIBRARY}.a"
            )
            add_dependencies(FFMPEG_${LIBRARY} FFMPEG_PROJECT)
        endforeach()

        set(FFMPEG_ENABLED 1 PARENT_SCOPE)
    else()
        message(WARNING "No support for video player on this platform")
    endif()
endfunction()
