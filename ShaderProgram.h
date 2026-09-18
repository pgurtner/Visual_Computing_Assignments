#ifndef VCASSIGNMENTS_SHADER_H
#define VCASSIGNMENTS_SHADER_H

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

class ShaderProgram
{
public:

    ShaderProgram (const std::string& vertexPath, const std::string& fragmentPath) {
        // vertex shader
        const std::string vertexCode = readFile(vertexPath);
        const char* vertexSource = vertexCode.c_str();

        const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

        glShaderSource(vertexShader, 1, &vertexSource, nullptr);
        glCompileShader(vertexShader);

        checkCompileErrors(vertexShader, "VERTEX");

        // fragment shader
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

    void use () const {
        glUseProgram(_id);
    }

    void setVec4 (const std::string& varName, const glm::vec4& value) const {
        const GLint varID = glGetUniformLocation(_id, varName.c_str());
        glUniform4fv(varID, 1, &value[0]);
    }

    void setMat4 (const std::string& varName, const glm::mat4& value) const {
        const GLint m_MVPID = glGetUniformLocation(_id, varName.c_str());
        glUniformMatrix4fv(m_MVPID, 1, GL_FALSE, &value[0][0]);
    }

private:
    GLuint _id;

    static std::string readFile(const std::string& path) {
        // std::ifstream file(path);
        //
        // if (!file.is_open()) {
        //     std::cerr << "Failed to open shader: "
        //               << path << "\n";
        // }
        //
        // std::stringstream buffer;
        // buffer << file.rdbuf();
        //
        // return buffer.str();
        std::ifstream file(path);
        std::string str;
        std::string file_contents;
        while (std::getline(file, str))
        {
            file_contents += str;
            file_contents.push_back('\n');
        }
        return file_contents;
    }

    static void checkCompileErrors (const GLuint shader, const std::string& type) {
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
};

#endif