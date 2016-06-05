cmake_minimum_required(VERSION 3.1)
project(REGoth)

add_subdirectory(lib/bgfx-cmake)
add_subdirectory(lib/ZenLib)

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
        "src/logic/*.cpp"
        "src/logic/visuals/*.cpp"
        "src/logic/scriptExternals/*.cpp"
        )

add_library(engine STATIC ${ENGINE_SRC})
set_target_properties(engine PROPERTIES LINKER_LANGUAGE C)
target_link_libraries(engine zenload daedalus vdfs utils bgfx bgfx_common bgfx3rdParty)

# ------------------ Other ------------------

include_directories(src)
include_directories(lib/ZenLib)
include_directories(lib/glm)
add_subdirectory(src/target)

