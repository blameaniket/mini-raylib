cmake_minimum_required(VERSION 3.10)
project(mini_raylib_app C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(app
    src/main.c
    src/app.c
    src/maths.c
    src/window.c
    src/camera.c
    src/renderer.c
    external/glad/src/glad.c
)

target_include_directories(app PRIVATE
    src
    external/glad/include
)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

target_link_libraries(app PRIVATE
    OpenGL::GL
    glfw
)


if(WIN32)
    target_link_libraries(app PRIVATE winmm)
endif()

if(NOT WIN32)
    target_link_libraries(app PRIVATE m)
endif()
