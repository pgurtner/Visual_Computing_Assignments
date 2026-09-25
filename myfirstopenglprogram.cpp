#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

#include <opencv2/opencv.hpp>

#include "ShaderProgram.h"

int main() {
    auto cam = cv::VideoCapture(0);
    if (!cam.isOpened()) {
        std::cout << "Error: Could not access the webcam." << std::endl;
        return -1;
    }
    std::cout << "Webcam accessed successfully!" << std::endl;

    const int frame_width = static_cast<int>(cam.get(cv::CAP_PROP_FRAME_WIDTH));
    const int frame_height = static_cast<int>(cam.get(cv::CAP_PROP_FRAME_HEIGHT));

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return 1;
    }

    const std::string windowName = "Window";
    const auto window = glfwCreateWindow(frame_width, frame_height, windowName.c_str(), nullptr, nullptr);
    if (window == nullptr) {
        fprintf(stderr, "Failed to open GLFW window. \n");
        getchar();
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    // setup cube ======================================================================================================
    std::vector<glm::vec3> cube = {
        {-1, -1, -1},
        {1, -1, -1},
        {1, 1, -1},
        {-1, 1, -1},

        {-1, -1, 1},
        {1, -1, 1},
        {1, 1, 1},
        {-1, 1, 1}
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

    const ShaderProgram cube_shaders("shaders/triangle.vert", "shaders/triangle.frag");

    GLuint cube_VAO;
    glGenVertexArrays(1, &cube_VAO);
    glBindVertexArray(cube_VAO);

    GLuint m_vertexBufferID;
    glGenBuffers(1, &m_vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, cube.size() * sizeof(glm::vec3), &cube[0], GL_STATIC_DRAW);

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
        (void *) 0 // array buffer offset
    );

    // setup camera stream =============================================================================================
    cv::Mat frame;
    cv::Mat filtered_frame;
    bool filter_active = false;

    const std::vector kernel_vec = {1.0/16, 2.0/16, 1.0/16, 2.0/16, 4.0/16, 2.0/16, 1.0/16, 2.0/16, 1.0/16};
    const cv::Mat gaussian_blur = cv::Mat(kernel_vec).reshape(1, 3);

    std::vector<float> camera_canvas = {
        // position      // UV
        -1.0f, -1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 0.0f
    };

    std::vector<GLuint> camera_canvas_indices = {
        0, 1, 2,
        2, 3, 0
    };

    const ShaderProgram camera_shaders("shaders/frame.vert", "shaders/frame.frag");

    GLuint camera_VAO;
    glGenVertexArrays(1, &camera_VAO);
    glBindVertexArray(camera_VAO);

    GLuint camera_VBO;
    glGenBuffers(1, &camera_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, camera_VBO);
    glBufferData(GL_ARRAY_BUFFER, camera_canvas.size() * sizeof(float), camera_canvas.data(), GL_STATIC_DRAW);

    GLuint camera_EBO;
    glGenBuffers(1, &camera_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, camera_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, camera_canvas_indices.size() * sizeof(GLint), camera_canvas_indices.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, camera_VBO);
    glVertexAttribPointer(
        0, // attribute
        2, // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        4 * sizeof(float),
        (void *) 0 // array buffer offset
    );
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        4 * sizeof(float),
        (void *) (2 * sizeof(float))
    );
    glEnableVertexAttribArray(1);

    // setup texture
    GLuint texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // according to Chattie, calling this with pixels=nullptr already allocates GPU memory, so this is intended
    // glTexImage2D can allocate memory, while glTexSubImage only updates
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        frame_width,
        frame_height,
        0,
        GL_BGR,
        GL_UNSIGNED_BYTE,
        nullptr
    );


    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw camera =================================================================================================
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
            filter_active = true;
        } else {
            filter_active = false;
        }

        glDisable(GL_DEPTH_TEST);

        auto ret = cam.read(frame);
        if (!ret) {
            std::cout << "Error: Could not capture frame." << std::endl;
        }

        if (filter_active) {
            cv::filter2D(frame, filtered_frame, -1, gaussian_blur);
        }

        glBindVertexArray(camera_VAO);

        camera_shaders.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        if (filter_active) {
            glTexSubImage2D(
                GL_TEXTURE_2D,
                0,
                0, 0,
                filtered_frame.cols,
                filtered_frame.rows,
                GL_BGR,
                GL_UNSIGNED_BYTE,
                filtered_frame.data
            );

        } else {
            glTexSubImage2D(
                GL_TEXTURE_2D,
                0,
                0, 0,
                frame.cols,
                frame.rows,
                GL_BGR,
                GL_UNSIGNED_BYTE,
                frame.data
            );
        }

        camera_shaders.setInt1("frameTexture", 0);

        glDrawElements(
            GL_TRIANGLES,
            6,
            GL_UNSIGNED_INT,
            nullptr
        );

        glClear(GL_DEPTH_BUFFER_BIT);

        // draw cube ===================================================================================================
        glEnable(GL_DEPTH_TEST);


        theta += 0.01;
        if (theta >= 2 * M_PI) {
            theta = 0.0;
        }

        glBindVertexArray(cube_VAO);

        cube_shaders.use();
        cube_shaders.setVec4("color", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

        glm::vec3 axis(0.0f, 0.0f, 1.0f);
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), theta, axis);

        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            static_cast<float>(width) / static_cast<float>(height),
            0.1f,
            100.0f
        );
        glm::mat4 view = glm::lookAt(
            glm::vec3(4, 3, 3),
            glm::vec3(0, 0, 0),
            glm::vec3(0, 1, 0)
        );
        glm::mat4 model = glm::mat4(1.0f);

        glm::mat4 MVPmatrix = projection * view * model * rotation;
        cube_shaders.setMat4("MVP", MVPmatrix);

        // todo use fragment coordinates in fragment shader color computation (for learning purposes)

        glDrawElements(
            GL_LINES,
            lineIndices.size(),
            GL_UNSIGNED_INT,
            nullptr
        );

        // general OpenGL stuff
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
