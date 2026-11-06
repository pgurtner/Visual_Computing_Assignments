cmake_minimum_required(VERSION 3.16)
project(VCAssignments LANGUAGES C CXX)

find_package(OpenCV REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(Assignment1
        assignment1.cpp
        external/glad/src/glad.c
        ShaderProgram.cpp
        ShaderProgram.h
)

add_executable(CameraCalibration
        calibration.cpp
)

add_executable(ImageFeatures
        features.cpp
)

add_executable(Assignment2
        assignment2.cpp
)

target_include_directories(Assignment1 PRIVATE
        external/glad/include
)

target_link_libraries(Assignment1
        PRIVATE
        ${OpenCV_LIBS}
        OpenGL::GL
        glfw
)

target_link_libraries(CameraCalibration
        PRIVATE
        ${OpenCV_LIBS}
)

target_link_libraries(ImageFeatures
        PRIVATE
        ${OpenCV_LIBS}
)

target_link_libraries(Assignment2
        PRIVATE
        ${OpenCV_LIBS}
)

add_custom_command(TARGET Assignment1 POST_BUILD # workaround for clion
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/shaders"
        "$<TARGET_FILE_DIR:Assignment1>/shaders"
)

add_custom_command(TARGET CameraCalibration POST_BUILD # workaround for clion
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/images"
        "$<TARGET_FILE_DIR:CameraCalibration>/images"
)

add_custom_command(TARGET ImageFeatures POST_BUILD # workaround for clion
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/images"
        "$<TARGET_FILE_DIR:ImageFeatures>/images"
)

add_custom_command(TARGET Assignment2 POST_BUILD # workaround for clion
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/images"
        "$<TARGET_FILE_DIR:Assignment2>/images"
)