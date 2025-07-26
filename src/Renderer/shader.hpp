#ifndef SHADER_HPP
#define SHADER_HPP

#include "file_utils.hpp"
#include "gl_utils.hpp"

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
private:
    uint32_t vertexShader, fragmentShader;
    uint32_t ID;

    std::string name = "";

    const std::string vertShaderPath;
    const std::string fragShaderPath;
public:

    Shader(const std::string& vertShaderPath, const std::string& fragShaderPath);
    ~Shader() {};

    void init();
    void use();

    void cleanup() {glDeleteProgram(ID);}

    template<typename T>
    void setUniform(const std::string& name, UniformType type, T value) {
        ::setUniform(ID, name, value);
    }

    void deleteProgram() {glDeleteProgram(ID);}

    uint32_t getID() const {return ID;}
    const std::string& getName() const {return name;}
    void setName(const std::string& shaderName) {name = shaderName;}

private:
    void createProgram();
    void compileShader(const std::string& code, uint32_t shaderType, uint32_t& shaderID);
    void checkCompileErrors(uint32_t shader, const std::string& type);
};

#endif // SHADER_HPP