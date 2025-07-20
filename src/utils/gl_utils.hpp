#ifndef GL_UTILS_HPP
#define GL_UTILS_HPP

#include <glad/glad.h>

#include <string>
#include <stdexcept>

enum class UniformType {
    INT,
    FLOAT,
    UINT,
    BOOL,
    VEC2,
    VEC3,
    VEC4,
    INT_VEC2,
    INT_VEC3,
    INT_VEC4,
    UINT_VEC2,
    UINT_VEC3,
    UINT_VEC4,
    BOOL_VEC2,
    BOOL_VEC3,
    BOOL_VEC4,
    MAT2,
    MAT3,
    MAT4,
    MAT2x3,
    MAT2x4,
    MAT3x2,
    MAT3x4,
    MAT4x2,
    MAT4x3, 
    // TODO: Add more types as needed
};

template<typename T>
void setUniform(GLuint program, const std::string& name, UniformType type, T value) {
    int location = glGetUniformLocation(program, name.c_str());
    if (location == -1) {
        throw std::runtime_error("Uniform '" + name + "' not found in shader program.");
    }
    switch (type) {
        case UniformType::INT:
            glUniform1i(location, value);
            break;
        case UniformType::FLOAT:
            glUniform1f(location, value);
            break;
        case UniformType::UINT:
            glUniform1ui(location, value);
            break;
        case UniformType::BOOL:
            glUniform1i(location, static_cast<int>(value));
            break;
        case UniformType::VEC2:
            glUniform2fv(location, 1, &value[0]);
            break;
        case UniformType::VEC3:
            glUniform3fv(location, 1, &value[0]);
            break;
        case UniformType::VEC4:
            glUniform4fv(location, 1, &value[0]);
            break;
        case UniformType::INT_VEC2:
            glUniform2iv(location, 1, &value[0]);
            break;
        case UniformType::INT_VEC3:
            glUniform3iv(location, 1, &value[0]);
            break;
        case UniformType::INT_VEC4:
            glUniform4iv(location, 1, &value[0]);
            break;
        case UniformType::UINT_VEC2:
            glUniform2uiv(location, 1, &value[0]);
            break;
        case UniformType::UINT_VEC3:
            glUniform3uiv(location, 1, &value[0]);
            break;
        case UniformType::UINT_VEC4:
            glUniform4uiv(location, 1, &value[0]);
            break;
        case UniformType::BOOL_VEC2:
            glUniform2iv(location, 1, &value[0]);
            break;
        case UniformType::BOOL_VEC3:
            glUniform3iv(location, 1, &value[0]);
            break;
        case UniformType::BOOL_VEC4:
            glUniform4iv(location, 1, &value[0]);
            break;
        case UniformType::MAT2:
            glUniformMatrix2fv(location, 1, GL_FALSE, &value[0][0]);
            break;
        case UniformType::MAT3:
            glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
            break;
        case UniformType::MAT4:
            glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
            break;
        case UniformType::MAT2x3:
            glUniformMatrix2x3fv(location, 1, GL_FALSE, &value[0][0]);
            break;
        case UniformType::MAT2x4:
            glUniformMatrix2x4fv(location, 1, GL_FALSE, &value[0][0]);
            break;
        case UniformType::MAT3x2:
            glUniformMatrix3x2fv(location, 1, GL_FALSE, &value[0][0]);
            break;
        case UniformType::MAT3x4:
            glUniformMatrix3x4fv(location, 1, GL_FALSE, &value[0][0]);
            break;
        case UniformType::MAT4x2:
            glUniformMatrix4x2fv(location, 1, GL_FALSE, &value[0][0]);
            break;
        case UniformType::MAT4x3:
            glUniformMatrix4x3fv(location, 1, GL_FALSE, &value[0][0]);
            break;
        default:
            throw std::runtime_error("Unsupported uniform type.");
    }
}

#endif // GL_UTILS_HPP