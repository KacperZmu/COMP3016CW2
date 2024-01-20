#include "Shader.h"
#include <iostream>

Shader::Shader(const char* vertexShaderSource, const char* fragmentShaderSource) {
    vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource, "VERTEX");
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource, "FRAGMENT");

    programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);

    checkShaderLinking();
}

Shader::~Shader() {
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteProgram(programID);
}

GLuint Shader::getProgramID() const {
    return programID;
}

void Shader::use() const {
    glUseProgram(programID);
}

GLuint Shader::compileShader(GLenum type, const char* source, const char* shaderType) {
    {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);
        
        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            GLchar infoLog[512];
            glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
            std::cerr << "Shader compilation error (" << shaderType << "): " << infoLog << std::endl;
            // Handle the error appropriately
        }

        return shader;
    }
}

void Shader::checkShaderLinking() {
    // Check for shader program linking errors
    GLint success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(programID, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Shader program linking error: " << infoLog << std::endl;
        // Handle the error appropriately
    }
}
