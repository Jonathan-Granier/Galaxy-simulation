########
# GLFW #
########

project(GLFW)

add_library(GLFW OBJECT)

target_compile_features(GLFW PRIVATE c_std_11)

# Defining preprocessor macros and selecting files to be removed

set(
    GLFW_DEFINITIONS

    -D_GLFW_WIN32
)

set(
    GLFW_PLATFORM_SRC

    glfw/src/egl*
    glfw/src/osmesa*
    glfw/src/wgl*
    glfw/src/win32*
)

if (OLYMPUS_PLATFORM_CYGWIN)
    # GLFW needs to be linked against the Windows Gdi32 library with Cygwin
    set(GLFW_LINKER_FLAGS Gdi32)
endif ()

file(
    GLOB
    GLFW_FILES

    # Common files
    glfw/src/context.c
    glfw/src/init.c
    glfw/src/input.c
    glfw/src/monitor.c
    glfw/src/vulkan.c
    glfw/src/window.c

    # Platform-specific files
    ${GLFW_PLATFORM_SRC}
)

# Building GLFW
target_sources(GLFW PRIVATE ${GLFW_FILES})

target_include_directories(
    GLFW

    SYSTEM
    PUBLIC

    glfw/include
    glfw/deps
)

target_compile_definitions(GLFW PUBLIC ${GLFW_DEFINITIONS})

# Disabling all compilers warnings
target_compile_options(GLFW PRIVATE -w)
target_link_libraries(GLFW PUBLIC ${GLFW_LINKER_FLAGS})
