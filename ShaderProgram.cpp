//
// Created by uni on 9/18/26.
//

#include "ShaderProgram.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

ShaderProgram::ShaderProgram (const std::string& vertexPath, const std::string& fragmentPath) {
    // vertex shader
    const std::string vertexCode = readFile(vertexPath);
    const char* vertexSource = vertexCode.c_str();

    const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);

    // todo: should throw exception to avoid hard to read errors when shader compilation fails
    checkCompileErrors(vertexShader, "VERTEX");

    // fragment shader
    // todo: can lead to memory leaks because vertexShader is not destroyed when readFile throws an exception
    const std::string fragmentCode = readFile(fragmentPath);
    const char* fragmentSource = fragmentCode.c_str();

    const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
    glCompileShader(fragmentShader);

    checkCompileErrors(fragmentShader, "FRAGMENT");

    // shader program
    _id = glCreateProgram();

    glAttachShader(_id, vertexShader);
    glAttachShader(_id, fragmentShader);

    glLinkProgram(_id);

    checkCompileErrors(_id, "PROGRAM");


    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(_id);
}

void ShaderProgram::use () const {
    glUseProgram(_id);
}

// todo glGetUniformLocation calls should be checked for error value
void ShaderProgram::setVec4 (const std::string& varName, const glm::vec4& value) const {
    const GLint varID = glGetUniformLocation(_id, varName.c_str());
    glUniform4fv(varID, 1, &value[0]);
}

void ShaderProgram::setMat4 (const std::string& varName, const glm::mat4& value) const {
    const GLint m_MVPID = glGetUniformLocation(_id, varName.c_str());
    glUniformMatrix4fv(m_MVPID, 1, GL_FALSE, &value[0][0]);
}

void ShaderProgram::setInt1 (const std::string& varName, int value) const {
    const GLint varId = glGetUniformLocation(_id, varName.c_str());
    glUniform1i(varId, value);
}

// from ChatGPT
std::string ShaderProgram::readFile(const std::string& path) {
    std::ifstream file(path);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open shader: " + path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

// from ChatGPT
void ShaderProgram::checkCompileErrors (const GLuint shader, const std::string& type) {
    GLint success;
    GLchar infoLog[1024];

    if (type != "PROGRAM") {
        glGetShaderiv(
            shader,
            GL_COMPILE_STATUS,
            &success
        );

        if (!success) {
            glGetShaderInfoLog(
                shader,
                1024,
                nullptr,
                infoLog
            );

            std::cerr
                << "Shader compilation error ("
                << type
                << "):\n"
                << infoLog
                << "\n";
        }
    } else {
        glGetProgramiv(
            shader,
            GL_LINK_STATUS,
            &success
        );

        if (!success) {
            glGetProgramInfoLog(
                shader,
                1024,
                nullptr,
                infoLog
            );

            std::cerr
                << "Shader linking error:\n"
                << infoLog
                << "\n";
        }
    }
}