#ifndef VCASSIGNMENTS_SHADER_H
#define VCASSIGNMENTS_SHADER_H

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

class ShaderProgram
{
public:
    ShaderProgram (const std::string& vertexPath, const std::string& fragmentPath);
    ~ShaderProgram();

    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;

    // has to be used before setting any uniforms because glUniformX works on the currently active shader program
    void use () const;

    void setVec4 (const std::string& varName, const glm::vec4& value) const;
    void setMat4 (const std::string& varName, const glm::mat4& value) const;
    void setInt1 (const std::string& varName, int value) const;

private:
    GLuint _id;

    static std::string readFile(const std::string& path);
    static void checkCompileErrors (GLuint shader, const std::string& type);
};

#endif