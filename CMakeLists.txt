cmake_minimum_required(VERSION 3.16)

cmake_path(GET CMAKE_CURRENT_SOURCE_DIR FILENAME ProjectName)
project(${ProjectName} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

set(Libs "C:/Projects/ThirdPartyLibs")

set(SDL3_DIR "${Libs}/SDL3/cmake")
find_package(SDL3 CONFIG REQUIRED)
set(SDL3_TTF_DIR "${Libs}/SDL3_ttf/cmake")
find_package(SDL3_TTF CONFIG REQUIRED)
set(imgui_DIR "${Libs}/imgui")
file(GLOB IMGUI_SOURCES CONFIGURE_DEPENDS ${imgui_DIR}/*.c* ${imgui_DIR}/*.h*)
set(IMGUI_BACKENDS
    "${imgui_DIR}/backends/imgui_impl_sdl3.h"
    "${imgui_DIR}/backends/imgui_impl_sdl3.cpp"
    "${imgui_DIR}/backends/imgui_impl_opengl3.h"
    "${imgui_DIR}/backends/imgui_impl_opengl3.cpp"
)
set(glm_DIR "${Libs}/glm/glm_install/share/glm")
list(APPEND CMAKE_PREFIX_PATH "${Libs}/glm/glm_install")
find_package(glm REQUIRED CONFIG)
find_package(OpenGL REQUIRED)
set(GLAD_DIR "${Libs}/glad")
file(GLOB_RECURSE GLAD_SOURCES CONFIGURE_DEPENDS ${GLAD_DIR}/*.c* ${GLAD_DIR}/*.h*)

file(GLOB_RECURSE sources LIST_DIRECTORIES FALSE RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS src/*.c* src/*.h*)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${sources})
source_group("imgui" FILES ${IMGUI_SOURCES} ${IMGUI_BACKENDS})
source_group("glad" FILES ${GLAD_SOURCES})

add_executable(${ProjectName} ${sources} ${IMGUI_SOURCES} ${IMGUI_BACKENDS} ${GLAD_SOURCES})

target_link_libraries(${ProjectName} PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf glm::glm OpenGL::GL)
target_include_directories(${ProjectName} PRIVATE src ${imgui_DIR} "${imgui_DIR}/backends" ${OPENGL_INCLUDE_DIR} "${GLAD_DIR}/include")
target_compile_options(${ProjectName} PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/MP>)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${ProjectName})

if(WIN32)
    add_custom_command(TARGET ${ProjectName} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:${ProjectName}>
        $<TARGET_FILE_DIR:${ProjectName}>
        COMMAND_EXPAND_LISTS
    )
endif()
