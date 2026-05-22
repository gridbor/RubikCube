cmake_minimum_required(VERSION 3.24)

cmake_path(GET CMAKE_CURRENT_SOURCE_DIR FILENAME ProjectName)
project(${ProjectName} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(GENERATOR_IS_MULTI_CONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()

option(FORMYSYSTEM "Loads libs from my system" OFF)

find_package(OpenGL REQUIRED)

if (FORMYSYSTEM)
    set(Libs "C:/Projects/ThirdPartyLibs")

    set(SDL3_DIR "${Libs}/SDL3/cmake")
    find_package(SDL3 CONFIG REQUIRED)
    set(SDL3_TTF_DIR "${Libs}/SDL3_ttf/cmake")
    find_package(SDL3_TTF CONFIG REQUIRED)
    set(glm_DIR "${Libs}/glm/glm_install/share/glm")
    list(APPEND CMAKE_PREFIX_PATH "${Libs}/glm/glm_install")
    find_package(glm REQUIRED CONFIG)
else()
    include(FetchContent)

    FetchContent_Declare(glm GIT_REPOSITORY "https://github.com/g-truc/glm.git" GIT_TAG 1.0.3)
    FetchContent_MakeAvailable(glm)
    set(SDL_TESTS OFF CACHE BOOL "" FORCE)
    set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(SDL3 GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git" GIT_TAG release-3.4.8)
    FetchContent_MakeAvailable(SDL3)
    FetchContent_Declare(SDL3_ttf GIT_REPOSITORY "https://github.com/libsdl-org/SDL_ttf.git" GIT_TAG release-3.2.2)
    FetchContent_MakeAvailable(SDL3_ttf)

    set(Libs "libs")
endif(FORMYSYSTEM)

add_library(glad STATIC "${Libs}/glad/src/glad.c")
target_include_directories(glad PUBLIC "${Libs}/glad/include")
target_link_libraries(glad PRIVATE OpenGL::GL)

add_library(imgui STATIC 
    "${Libs}/imgui/imgui.cpp"
    "${Libs}/imgui/imgui_draw.cpp"
    "${Libs}/imgui/imgui_tables.cpp"
    "${Libs}/imgui/imgui_widgets.cpp"
    "${Libs}/imgui/backends/imgui_impl_sdl3.cpp"
    "${Libs}/imgui/backends/imgui_impl_opengl3.cpp"
)
target_include_directories(imgui PUBLIC "${Libs}/imgui" "${Libs}/imgui/backends")
target_link_libraries(imgui PRIVATE SDL3::SDL3)

file(GLOB_RECURSE sources LIST_DIRECTORIES FALSE RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS src/*.c* src/*.h*)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${sources})

add_executable(${ProjectName} ${sources}) 

target_link_libraries(${ProjectName} PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf glm::glm OpenGL::GL glad imgui)
target_include_directories(${ProjectName} PRIVATE src ${OPENGL_INCLUDE_DIR})
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
