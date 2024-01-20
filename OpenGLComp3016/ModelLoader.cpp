#include "ModelLoader.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iostream>

std::vector<Vertex> ModelLoader::loadOBJModel(const std::string& filePath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Assimp error: " << importer.GetErrorString() << std::endl;
        return {};  // Return an empty vector to indicate failure
    }

    std::vector<Vertex> vertices;


    const aiMesh* mesh = scene->mMeshes[0];

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        Vertex vertex;

        vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        if (mesh->HasVertexColors(0)) {
            // Use vertex colors if available
            vertex.color = glm::vec3(mesh->mColors[0][i].r, mesh->mColors[0][i].g, mesh->mColors[0][i].b);
        }
        else {
            // Otherwise, assign a default color
            vertex.color = glm::vec3(1.0f, 0.0f, 0.0f);
        }
        if (mesh->HasNormals()) {
            glm::vec3 normal(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            // Handle normals
        }
        vertices.push_back(vertex);
    }

    // Print some debug info
    std::cout << "Number of vertices loaded (OBJ): " << vertices.size() << std::endl;

    // Output the first few vertices
    for (int i = 0; i < std::min(5, static_cast<int>(vertices.size())); ++i) {
        std::cout << "Vertex " << i << ": " << vertices[i].position.x << ", "
            << vertices[i].position.y << ", " << vertices[i].position.z << std::endl;
    }

    return vertices;
}

// Load FBX model using Assimp
std::vector<Vertex> ModelLoader::loadFBXModel(const std::string& filePath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Assimp error: " << importer.GetErrorString() << std::endl;
        return {};  // Return an empty vector to indicate failure
    }

    std::vector<Vertex> vertices;


    const aiMesh* mesh = scene->mMeshes[0];

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        Vertex vertex;

        vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        if (mesh->HasVertexColors(0)) {
            // Use vertex colors if available
            vertex.color = glm::vec3(mesh->mColors[0][i].r, mesh->mColors[0][i].g, mesh->mColors[0][i].b);
        }
        else {
            // Otherwise, assign a default color
            vertex.color = glm::vec3(1.0f, 0.0f, 0.0f);
        }
        if (mesh->HasNormals()) {
            glm::vec3 normal(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            // Handle normal
        }
        vertices.push_back(vertex);
    }

    // Print some debug inf
    std::cout << "Number of vertices loaded (FBX): " << vertices.size() << std::endl;

    // Output the first few vertices
    for (int i = 0; i < std::min(5, static_cast<int>(vertices.size())); ++i) {
        std::cout << "Vertex " << i << ": " << vertices[i].position.x << ", "
            << vertices[i].position.y << ", " << vertices[i].position.z << std::endl;
    }

    for (auto& vertex : vertices) {
        // Translate 5.0f units to the right and 5.0f units behind
        vertex.position.x += 10.0f;
        vertex.position.z -= 5.0f;

    }

    return vertices;
}


// Load DAE model using Assimp
std::vector<Vertex> ModelLoader::loadDAEModel(const std::string& filePath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Assimp error: " << importer.GetErrorString() << std::endl;
        return {};  // Return an empty vector to indicate failure
    }

    std::vector<Vertex> vertices;


    const aiMesh* mesh = scene->mMeshes[0];

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        Vertex vertex;

        vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        if (mesh->HasVertexColors(0)) {
            // Use vertex colors if available
            vertex.color = glm::vec3(mesh->mColors[0][i].r, mesh->mColors[0][i].g, mesh->mColors[0][i].b);
        }
        else {
            // Otherwise, assign a default color
            vertex.color = glm::vec3(1.0f, 0.0f, 0.0f);
        }
        if (mesh->HasNormals()) {
            // Handle normals
            glm::vec3 normal(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

        }
        vertices.push_back(vertex);
    }
    for (auto& vertex : vertices) {
        // Move the DAE model 6.0f units back
        vertex.position.z -= 6.0f;
    }

    // Print some debug information
    std::cout << "Number of vertices loaded (DAE): " << vertices.size() << std::endl;

    // Output the first few vertices
    for (int i = 0; i < std::min(5, static_cast<int>(vertices.size())); ++i) {
        std::cout << "Vertex " << i << ": " << vertices[i].position.x << ", "
            << vertices[i].position.y << ", " << vertices[i].position.z << std::endl;
    }



    return vertices;
}
