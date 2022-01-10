cmake_minimum_required(VERSION 3.0.0)
project(Galaxy)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/extern/Olympus/cmake")

add_executable(Galaxy)

# Using C++17
target_compile_features(Galaxy PRIVATE cxx_std_17)

############################
# Olympus - Compiler flags #
############################ 

include(CompilerFlags) 
add_compiler_flags(Galaxy PRIVATE)
 
  
#############################
# Olympus - Finding Vulkan #
############################# 
   
if (NOT VULKAN_SDK_ROOT)
    # Vulkan SDK path not set, trying to find it automatically
    find_package(Vulkan) 
endif ()

if (NOT Vulkan_FOUND)
    # If not found automatically by find_package(), try another way

    if (NOT VULKAN_SDK_ROOT AND OLYMPUS_PLATFORM_WINDOWS)
        # If under Windows with the path not manually set, try to locate it from the environment variable
        set(VULKAN_SDK_ROOT "$ENV{VK_SDK_PATH}")
    endif () 

    if (VULKAN_SDK_ROOT)
        # SDK path has been defined

        if (OLYMPUS_PLATFORM_WINDOWS)
            string(REPLACE "\\" "/" VULKAN_SDK_ROOT ${VULKAN_SDK_ROOT})
            set(Vulkan_INCLUDE_DIRS "${VULKAN_SDK_ROOT}/Include")
            set(Vulkan_LIBRARIES "${VULKAN_SDK_ROOT}/Lib/vulkan-1.lib")
        else ()
            set(Vulkan_INCLUDE_DIRS "${VULKAN_SDK_ROOT}/x86_64/include")
            set(Vulkan_LIBRARIES "${VULKAN_SDK_ROOT}/x86_64/lib/vulkan")
        endif ()

        find_program(GLSLC_EXECUTABLE glslc HINTS "${VULKAN_SDK_ROOT}/bin")

        if (EXISTS "${Vulkan_INCLUDE_DIRS}/vulkan/vulkan.h" AND EXISTS "${Vulkan_LIBRARIES}")
            set(Vulkan_FOUND ON)
        endif () 
    endif ()
endif () 
  
if (Vulkan_FOUND)
    if (NOT GLSLC_EXECUTABLE)
        set(GLSLC_EXECUTABLE glslc) # glslc is assumed to be available from the path... Kind of a long shot under Windows
    endif () 

    if (GLSLC_EXECUTABLE)
        #######################
        # Shaders compilation #
        #######################

        set(SHADERS_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/shaders")

        # Recovering shaders
        file(
            GLOB_RECURSE
            OLYMPUS_SHADERS

            ${SHADERS_ROOT}/*.vert # Vertex shader
            ${SHADERS_ROOT}/*.tesc # Tesselation control shader
            ${SHADERS_ROOT}/*.tese # Tesselation evaluation shader
            ${SHADERS_ROOT}/*.geom # Geometry shader
            ${SHADERS_ROOT}/*.frag # Fragment shader
            ${SHADERS_ROOT}/*.comp # Compute shader
        )

        # Compiling all shaders found
        foreach (SHADER_INPUT_PATH ${OLYMPUS_SHADERS})
            get_filename_component(SHADER_FILENAME ${SHADER_INPUT_PATH} NAME) # Stripping the path from the prepending folders, keeping the file's name
            string(REPLACE "." ";" SHADER_FILE_PARTS ${SHADER_FILENAME})

            list(GET SHADER_FILE_PARTS 0 SHADER_NAME)
            list(GET SHADER_FILE_PARTS 1 SHADER_EXT)

            set(SHADER_OUTPUT_PATH "${SHADERS_ROOT}/build/${SHADER_NAME}_${SHADER_EXT}.spv")

            add_custom_command(
                OUTPUT "${SHADER_OUTPUT_PATH}"
                COMMAND ${GLSLC_EXECUTABLE} ${SHADER_INPUT_PATH} -o ${SHADER_OUTPUT_PATH}
                DEPENDS "${SHADER_INPUT_PATH}"
                WORKING_DIRECTORY "${SHADERS_ROOT}"
                COMMENT "Compiling shader ${SHADER_FILENAME} to SPIR-V"
                VERBATIM
            )

            target_sources(Galaxy PRIVATE "${SHADER_OUTPUT_PATH}")
        endforeach ()
    else ()
        # GLSLC executable not found, send a warning

        set(GLSLC_EXECUTABLE "" CACHE PATH "Path to your GLSLC executable")
        message(WARNING "[Olympus] Warning: GLSLC executable not found. To compile shaders into SPIR-V, please specify its path manually.")
    endif ()
else ()
    # Neither method has found a match, send an error

    set(VULKAN_SDK_ROOT "" CACHE PATH "Path to your Vulkan SDK installation")
    message(SEND_ERROR "[Olympus] Error: Vulkan SDK not found, please specify its installation path manually.")
endif ()

##########################
# GALAXY - Source files #
##########################

set(
    GALAXY_FILES

    sources/*.cpp
    include/*.h
    include/*.inl
)   
   
# Adding every file to be compiled
file(
    GLOB_RECURSE
    GALAXY_SOURCES 
  
    ${GALAXY_FILES}
) 

####################
# Galaxy - Build #
####################

target_sources(Galaxy PRIVATE ${GALAXY_SOURCES})

target_include_directories(
	Galaxy

	PRIVATE

	${CMAKE_CURRENT_SOURCE_DIR}/include
)
    
add_subdirectory(extern)
add_subdirectory(extern/glm)
set(
    GALAXY_LINKER_FLAGS 

    ${GALAXY_LINKER_FLAGS}
    GLFW 
    ImGui
    glm::glm
    Olympus
)  

target_compile_definitions(
    Galaxy
  
    PRIVATE  

    GALAXY_SHADERS="${CMAKE_CURRENT_SOURCE_DIR}/shaders/build"
    GLFW_INCLUDE_VULKAN #use vulkan with GLFW
    GLM_ENABLE_EXPERIMENTAL # To be able to use quaternions
    GLM_FORCE_RADIANS # Forcing calculations to be in radians
    GLM_FORCE_DEPTH_ZERO_TO_ONE # To fix glm's perspective matrix
    #GLM_FORCE_LEFT_HANDED # Needs to be forced to LH for Vulkan
)

target_compile_options(Galaxy PRIVATE ${GALAXY_COMPILER_FLAGS})
target_link_libraries(Galaxy PRIVATE ${GALAXY_LINKER_FLAGS})
