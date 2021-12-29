#########
# ImGui #
#########

project(ImGui)

add_library(ImGui OBJECT)

target_compile_features(ImGui PRIVATE cxx_std_17)

# The needed Freetype header ft2build.h seems to be found only when calling aux_source_directory(); this call is thus kept instead of manually finding files
aux_source_directory(imgui IMGUI_SRC)

set(
    IMGUI_SRC

    ${IMGUI_SRC}
    imgui/*.h
    #imgui/*.cpp
    imgui/misc/cpp/*.cpp
)

file(
    GLOB_RECURSE
    IMGUI_FILES

    ${IMGUI_SRC}
)

# Building ImGui
target_sources(ImGui PRIVATE ${IMGUI_FILES})

target_include_directories(
    ImGui

    SYSTEM
    PUBLIC

    ${CMAKE_CURRENT_SOURCE_DIR}

    PRIVATE

    ${CMAKE_CURRENT_SOURCE_DIR}/imgui # Needed for imgui_stdlib.cpp
    glew/include
)

# Disabling all compilers warnings
if (OLYMPUS_COMPILER_MSVC)
    target_compile_options(ImGui PRIVATE /w)
else ()
    target_compile_options(ImGui PRIVATE -w)
endif ()

target_link_libraries(ImGui PUBLIC GLFW)

