cmake_minimum_required(VERSION 3.1)
project(REGoth)

set(CMAKE_CXX_STANDARD 14)

add_definitions(-DBGFX_CONFIG_DEBUG=1)

# Fix up C++ standard
if (CMAKE_COMPILER_IS_GNUCC)
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 6.0)
        message(WARNING 'Compilation wasn't tested on GCC versions prior to 6.x and might not be successful!')
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")
    endif()
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(GAME_CONTENT_DIR ../content CACHE STRING "Content-folder, containing VDF-Archives and other files.")
set(GAME_STARTUP_WORLD addonworld.zen CACHE STRING "[Testing] World to load at program start")

# Setup BGFX
add_definitions(-DBGFX_CONFIG_MAX_MATRIX_CACHE=231072) # Need to up this since everything got it's own matrix right now
add_definitions(-DBGFX_CONFIG_MAX_COMMAND_BUFFER_SIZE=231072) # FIXME: Flush some times at loading, so we don't exhaust the default setting of 64k
add_definitions(-DBGFX_CONFIG_MAX_INDEX_BUFFERS=65535)
add_definitions(-DBGFX_CONFIG_MAX_VERTEX_BUFFERS=65535) # TODO: Don't mess around with memory so much

add_definitions(-DGLM_FORCE_PURE) # FIXME: glm crashes using SSE because of some alignment issues

# Temp
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

if(MINGW)
    SET(COMPILER_PREFIX "i686-mingw32")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s")
endif()

if(MSVC)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /MP")
	add_definitions(-DNOMINMAX)
endif()

# Setup OpenMP
FIND_PACKAGE(OpenMP)
if(OPENMP_FOUND)
    message("OPENMP FOUND")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()


# ------------------ Config-Header ------------------
configure_file(src/config.h.in ${PROJECT_BINARY_DIR}/src/config.h)
include_directories(${PROJECT_BINARY_DIR}/src)

add_subdirectory(lib/bgfx-cmake)
add_subdirectory(lib/ZenLib)

# Set up bullet
set(USE_GRAPHICAL_BENCHMARK OFF)
set(USE_MSVC_RUNTIME_LIBRARY_DLL ON) # Don't statically link this
set(BUILD_BULLET2_DEMOS OFF) # No samples
set(BUILD_OPENGL3_DEMOS OFF) # No samples
set(BUILD_UNIT_TESTS OFF) # No tests
set(BUILD_CPU_DEMOS OFF) # No samples

add_subdirectory(lib/bullet3)
include_directories(lib/bullet3/src)

set(BULLET_LIBRARIES Bullet3Common BulletSoftBody BulletDynamics BulletCollision BulletInverseDynamicsUtils BulletInverseDynamics LinearMath)
set(BULLET_MISC Bullet3Dynamics Bullet2FileLoader Bullet3Collision Bullet3Geometry Bullet3OpenCL_clew BulletFileLoader BulletWorldImporter BulletXmlWorldImporter ConvexDecomposition GIMPACTUtils HACD Test_BulletDynamics Test_BulletInverseDynamics Test_Collision)


# ------------------ Engine lib ------------------
file(GLOB ENGINE_SRC
        "src/components/*.cpp"
        "src/engine/*.cpp"
        "src/handle/*.cpp"
        "src/memory/*.cpp"
        "src/content/*.cpp"
        "src/render/*.cpp"
        "src/utils/*.cpp"
        "src/math/*.cpp"
        "src/ui/*.cpp"
        "src/logic/*.cpp"
        "src/physics/*.cpp"
        "src/logic/visuals/*.cpp"
        "src/logic/scriptExternals/*.cpp"
        )

add_library(engine STATIC ${ENGINE_SRC})
set_target_properties(engine PROPERTIES LINKER_LANGUAGE C)
target_link_libraries(engine zenload daedalus vdfs utils bgfx bgfx_common bgfx3rdParty ${BULLET_LIBRARIES})

# ------------------ Other ------------------

include_directories(src)
include_directories(lib/ZenLib)
include_directories(lib/glm)
include_directories(lib/tinydir)
add_subdirectory(src/target)

