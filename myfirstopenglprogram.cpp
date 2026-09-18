#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

#include "ShaderProgram.h"

int main()
{
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return 1;
    }

    std::string windowName = "Window";
    // Open a window and create its OpenGL context
    auto window = glfwCreateWindow( 1024, 768, windowName.c_str(), nullptr, nullptr);
    if( window == nullptr ){
        fprintf( stderr, "Failed to open GLFW window. \n" );
        getchar();
        glfwTerminate();
        return 1;
    }
    // set the context as current
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    std::vector<glm::vec3> cube = {
        {-1, -1, -1},
        { 1, -1, -1},
        { 1,  1, -1},
        {-1,  1, -1},

        {-1, -1,  1},
        { 1, -1,  1},
        { 1,  1,  1},
        {-1,  1,  1}
    };
    double theta = 0.0;

    const ShaderProgram shaders("shaders/triangle.vert", "shaders/triangle.frag");


    while (!glfwWindowShouldClose(window)) {
        theta = std::fmod(theta + 0.1, 2.0*M_PI);

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        // - - - - - - - - - - - - 1. Step: Creating the data - - - - - - - - - - - - - - - - -

        // Load it into a VBO
        GLuint m_vertexBufferID;
        glGenBuffers(1, &m_vertexBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferID);
        glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec3), &m_vertices[0], GL_STATIC_DRAW);
        // - - - - - - - - - - - - 2.Step: Using the data for doing the rendering - - - - - - - - - - - - - - -
        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferID);
        glVertexAttribPointer(
        0, // attribute
        3, // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0, // stride - 0= tightly packed
        (void*)0 // array buffer offset
        );
        // - - - - - - - - - - - - Actual drawing call - - - - - - - -

        shaders.use();
        shaders.setVec4("color", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

        float angle = glm::radians(theta);
        glm::vec3 axis(0.0f, 0.0f, 1.0f);
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, axis);
        shaders.setMat4("R", rotation);
        // todo do 3d -> 2d projection

        glDrawArrays(GL_TRIANGLES, 0, m_vertices.size()); //draw x triangles

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
