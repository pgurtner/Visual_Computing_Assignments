#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

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
    std::vector<GLuint> indices = {
        // Back
        0, 1, 2,
        2, 3, 0,

        // Front
        4, 5, 6,
        6, 7, 4,

        // Left
        0, 3, 7,
        7, 4, 0,

        // Right
        1, 5, 6,
        6, 2, 1,

        // Bottom
        0, 4, 5,
        5, 1, 0,

        // Top
        3, 2, 6,
        6, 7, 3
    };

    std::vector<GLuint> lineIndices = {
        // Back face
        0, 1,
        1, 2,
        2, 3,
        3, 0,

        // Front face
        4, 5,
        5, 6,
        6, 7,
        7, 4,

        // Connecting edges
        0, 4,
        1, 5,
        2, 6,
        3, 7
    };

    float theta = 0.0;

    const ShaderProgram shaders("shaders/triangle.vert", "shaders/triangle.frag");

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    // - - - - - - - - - - - - 1. Step: Creating the data - - - - - - - - - - - - - - - - -

    // Load it into a VBO
    GLuint m_vertexBufferID;
    glGenBuffers(1, &m_vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, cube.size() * sizeof(glm::vec3), &cube[0], GL_STATIC_DRAW);
    // - - - - - - - - - - - - 2.Step: Using the data for doing the rendering - - - - - - - - - - - - - - -
    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);

    // group vertices into faces
    GLuint indexBufferID;
    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, lineIndices.size() * sizeof(GLuint), lineIndices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);



    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferID);
    glVertexAttribPointer(
        0, // attribute
        3, // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
            sizeof(glm::vec3), // stride - 0= tightly packed
        (void*)0 // array buffer offset
    );



    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        theta += 0.01;
        if (theta >= 2*M_PI) {
            theta = 0.0;
        }

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaders.use();
        shaders.setVec4("color", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));


        glm::vec3 axis(0.0f, 0.0f, 1.0f);
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), theta, axis);

        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
        static_cast<float>(width) / static_cast<float>(height),
        0.1f,
        100.0f
        );
        glm::mat4 view = glm::lookAt(
            glm::vec3(4,3,3),
            glm::vec3(0,0,0),
            glm::vec3(0,1,0)
            );
        glm::mat4 model = glm::mat4(1.0f);

        glm::mat4 MVPmatrix = projection * view * model * rotation;
        shaders.setMat4("MVP", MVPmatrix);



        // todo use fragment coordinates in fragment shader color computation (for learning purposes)

        glDrawElements(
            GL_LINES,
            lineIndices.size(),
            GL_UNSIGNED_INT,
            nullptr
        );
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
