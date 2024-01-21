#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include "Shader.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
};

class ModelLoader {
public:
    static std::vector<Vertex> loadOBJModel(const std::string& filePath);
    static std::vector<Vertex> loadFBXModel(const std::string& filePath);
    static std::vector<Vertex> loadDAEModel(const std::string& filePath);
};

