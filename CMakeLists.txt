cmake_minimum_required(VERSION 3.16)
project(VCAssignments LANGUAGES C CXX)

find_package(OpenCV REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(MyProject main.cpp)
target_link_libraries(MyProject PRIVATE ${OpenCV_LIBS})

add_executable(MyOpenGLProject
        myfirstopenglprogram.cpp
        external/glad/src/glad.c
        ShaderProgram.cpp
        ShaderProgram.h
)

target_include_directories(MyOpenGLProject PRIVATE
        external/glad/include
)

target_link_libraries(MyOpenGLProject
        PRIVATE
        OpenGL::GL
        glfw
)


add_custom_command(TARGET MyOpenGLProject POST_BUILD # workaround for clion
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/shaders"
        "$<TARGET_FILE_DIR:MyOpenGLProject>/shaders"
)