#pragma once

#include <GL/glew.h>

class Shader {
public:
    Shader(const char* vertexShaderSource, const char* fragmentShaderSource);
    ~Shader();
    GLuint getProgramID() const;
    void use() const;

private:
    GLuint programID;
    GLuint vertexShader;
    GLuint fragmentShader;

    GLuint compileShader(GLenum type, const char* source, const char* shaderType);
    void checkShaderLinking();
};

