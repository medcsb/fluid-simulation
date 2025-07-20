#ifndef GL_UTILS_HPP
#define GL_UTILS_HPP

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <stdexcept>

enum class TextureParameter {

};

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

inline void setUniform(GLuint program, const std::string& name, int value) {
    int location = glGetUniformLocation(program, name.c_str());
    if (location == -1) {
        throw std::runtime_error("Uniform '" + name + "' not found in shader program.");
    }
    glUniform1i(location, value);
}

inline void setUniform(GLuint program, const std::string& name, float value) {
    int location = glGetUniformLocation(program, name.c_str());
    if (location == -1) {
        throw std::runtime_error("Uniform '" + name + "' not found in shader program.");
    }
    glUniform1f(location, value);
}

inline void setUniform(GLuint program, const std::string& name, unsigned int value) {
    int location = glGetUniformLocation(program, name.c_str());
    if (location == -1) {
        throw std::runtime_error("Uniform '" + name + "' not found in shader program.");
    }
    glUniform1ui(location, value);
}

inline void setUniform(GLuint program, const std::string& name, bool value) {
    int location = glGetUniformLocation(program, name.c_str());
    if (location == -1) {
        throw std::runtime_error("Uniform '" + name + "' not found in shader program.");
    }
    glUniform1i(location, static_cast<int>(value));
}

inline void setUniform(GLuint program, const std::string& name, const glm::vec2& value) {
    int location = glGetUniformLocation(program, name.c_str());
    if (location == -1) {
        throw std::runtime_error("Uniform '" + name + "' not found in shader program.");
    }
    glUniform2fv(location, 1, glm::value_ptr(value));
}

inline void setUniform(GLuint program, const std::string& name, const glm::vec3& value) {
    int location = glGetUniformLocation(program, name.c_str());
    if (location == -1) {
        throw std::runtime_error("Uniform '" + name + "' not found in shader program.");
    }
    glUniform3fv(location, 1, glm::value_ptr(value));
}

inline void setUniform(GLuint program, const std::string& name, const glm::vec4& value) {
    int location = glGetUniformLocation(program, name.c_str());
    if (location == -1) {
        throw std::runtime_error("Uniform '" + name + "' not found in shader program.");
    }
    glUniform4fv(location, 1, glm::value_ptr(value));
}

inline void setUniform(GLuint program, const std::string& name, const glm::mat2& value) {
    int location = glGetUniformLocation(program, name.c_str());
    if (location == -1) {
        throw std::runtime_error("Uniform '" + name + "' not found in shader program.");
    }
    glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

inline void setUniform(GLuint program, const std::string& name, const glm::mat3& value) {
    int location = glGetUniformLocation(program, name.c_str());
    if (location == -1) {
        throw std::runtime_error("Uniform '" + name + "' not found in shader program.");
    }
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

inline void setUniform(GLuint program, const std::string& name, const glm::mat4& value) {
    int location = glGetUniformLocation(program, name.c_str());
    if (location == -1) {
        throw std::runtime_error("Uniform '" + name + "' not found in shader program.");
    }
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

#endif // GL_UTILS_HPP