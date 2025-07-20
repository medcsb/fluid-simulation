#include "shader.hpp"

Shader::Shader(const std::string& vertShaderPath, const std::string& fragShaderPath) 
    : vertShaderPath(vertShaderPath), fragShaderPath(fragShaderPath) {}

void Shader::init() {
    std::string vertCodeStr;
    std::string fragCodeStr;
    readFile(vertShaderPath, vertCodeStr);
    readFile(fragShaderPath, fragCodeStr);

    compileShader(vertCodeStr, GL_VERTEX_SHADER, vertexShader);
    checkCompileErrors(vertexShader, "Vertex");
    compileShader(fragCodeStr, GL_FRAGMENT_SHADER, fragmentShader);
    checkCompileErrors(fragmentShader, "Fragment");

    createProgram();
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::use() {
    glUseProgram(ID);
}

template<typename T>
void Shader::setUniform(const std::string& name, UniformType type, T value) {
    setUniform(ID, name, type, value);
}

void Shader::createProgram() {
    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);
    
    checkCompileErrors(ID, "Program");
}

void Shader::compileShader(const std::string& code, uint32_t shaderType, uint32_t& shaderID) {
    shaderID = glCreateShader(shaderType);
    const char* codeCStr = code.c_str();
    glShaderSource(shaderID, 1, &codeCStr, nullptr);
    glCompileShader(shaderID);
}

void Shader::checkCompileErrors(uint32_t shader, const std::string& type) {
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        throw std::runtime_error(type + " Shader compilation failed: " + std::string(infoLog));
    }
}