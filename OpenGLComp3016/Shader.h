#pragma once

#include <GL/glew.h>

class Shader {
public:
    // constructor takes shaders source code
    Shader(const char* vertexShaderSource, const char* fragmentShaderSource);
    ~Shader();
    GLuint getProgramID() const;
    void use() const;

private:
    GLuint programID;
    GLuint vertexShader;
    GLuint fragmentShader;

    // functions for compiling and shader linking check
    GLuint compileShader(GLenum type, const char* source, const char* shaderType);
    void checkShaderLinking();
};

