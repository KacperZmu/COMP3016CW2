#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <cmath>
#include <iostream>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

double lastX = 400.0;
double lastY = 300.0;
float yaw = 0.0f;
float pitch = 0.0f;
GLuint vertexShader, fragmentShader;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);




GLfloat shapeVertices[] = {
    // Cube vertices
    -0.5f, -0.5f, -0.5f,  // Bottom left
     0.5f, -0.5f, -0.5f,  // Bottom right
     0.5f, -0.5f,  0.5f,  // Top right

    -0.5f, -0.5f, -0.5f,  // Bottom left
     0.5f, -0.5f,  0.5f,  // Top right
    -0.5f, -0.5f,  0.5f,  // Top left

    // Faces
    0.0f,  0.5f,  0.0f,  // Apex
   -0.5f, -0.5f,  0.5f,  // Top left of the base
    0.5f, -0.5f,  0.5f,  // Top right of the base

    0.0f,  0.5f,  0.0f,  // Apex
    0.5f, -0.5f,  0.5f,  // Top right of the base
    0.5f, -0.5f, -0.5f,  // Bottom right of the base

    0.0f,  0.5f,  0.0f,  // Apex
    0.5f, -0.5f, -0.5f,  // Bottom right of the base
   -0.5f, -0.5f, -0.5f,  // Bottom left of the base

    0.0f,  0.5f,  0.0f,  // Apex
   -0.5f, -0.5f, -0.5f,  // Bottom left of the base
   -0.5f, -0.5f,  0.5f,  // Top left of the base

   // Connecting faces (sides of the pyramid)
  -0.5f, -0.5f,  0.5f,  // Top left of the base
   0.5f, -0.5f,  0.5f,  // Top right of the base
   0.0f,  0.5f,  0.0f,  // Apex

   0.5f, -0.5f,  0.5f,  // Top right of the base
   0.5f, -0.5f, -0.5f,  // Bottom right of the base
   0.0f,  0.5f,  0.0f,  // Apex

   0.5f, -0.5f, -0.5f,  // Bottom right of the base
  -0.5f, -0.5f, -0.5f,  // Bottom left of the base
   0.0f,  0.5f,  0.0f,  // Apex

  -0.5f, -0.5f, -0.5f,  // Bottom left of the base
  -0.5f, -0.5f,  0.5f,  // Top left of the base
   0.0f,  0.5f,  0.0f,  // Apex

   // Left face
  -0.5f, -0.5f, -0.5f,  // Bottom left of the base
  -0.5f, -0.5f,  0.5f,  // Top left of the base
   0.0f,  0.5f,  0.0f,  // Apex

   // Back face
    0.5f, -0.5f, -0.5f,  // Bottom right of the base
   -0.5f, -0.5f, -0.5f,  // Bottom left of the base
    0.0f,  0.5f,  0.0f   // Apex
};



const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    layout (location = 2) in vec3 aNormal; 

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    out vec3 FragPos; // Pass position to fragment shader
    out vec3 Normal; // Pass normal to fragment shader
    out vec3 Color; // Pass color to fragment shader

    void main()
    {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        FragPos = vec3(model * vec4(aPos, 1.0));
        Normal = mat3(transpose(inverse(model))) * aNormal;
        Color = aColor;
    }
)";


const char* fragmentShaderSource = R"(
    #version 330 core
    in vec3 FragPos; 
    in vec3 Normal; 
    in vec3 Color; 

    out vec4 FragColorOutput;

    uniform vec3 lightPos; 
    uniform vec3 viewPos; 

    void main()
    {
        // Ambient lighting
        float ambientStrength = 0.3;
        vec3 ambient = ambientStrength * Color;

        // Diffuse lighting
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(Normal, lightDir), 0.0);
        vec3 diffuse = diff * Color;

        // Specular lighting
        float specularStrength = 0.5;
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, Normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
        vec3 specular = specularStrength * spec * vec3(1.0, 1.0, 1.0);

        // Final color
        vec3 result = ambient + diffuse + specular;
        FragColorOutput = vec4(result, 1.0);
    }
)";


struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
};

// Load OBJ model using Assimp
std::vector<Vertex> loadOBJModel(const std::string& filePath) {
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
std::vector<Vertex> loadFBXModel(const std::string& filePath) {
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
std::vector<Vertex> loadDAEModel(const std::string& filePath) {
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




void processInput(GLFWwindow* window, float deltaTime) {
    const float cameraSpeed = 2.5f * deltaTime;

    // Handle mouse input to change yaw and pitch
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    const float sensitivity = 0.05f;
    yaw += static_cast<float>(sensitivity * (mouseX - lastX));
    pitch -= static_cast<float>(sensitivity * (lastY - mouseY));

    lastX = mouseX;
    lastY = mouseY;

    // Update camera front vector
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);

    glm::vec3 right = glm::normalize(glm::cross(cameraFront, cameraUp));
    glm::vec3 up = glm::normalize(glm::cross(right, cameraFront));

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos += cameraSpeed * right;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos -= cameraSpeed * right;
    cameraPos.y = 0.0f;
}




GLuint compileShader(GLenum type, const char* source, const char* shaderType)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    // Check for shader compilation errors
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





int main(void)
{
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(1200, 800, "OpenGL Window", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    glfwMakeContextCurrent(window);
    glEnable(GL_DEPTH_TEST);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    std::vector<Vertex> OBJVertices = loadOBJModel("H:/Comp3016/COMP3016CW2/COMP3016CW2/OpenGLComp3016/lowpolysword1.obj");
    std::vector<Vertex> FBXVertices = loadFBXModel("H:/Comp3016/COMP3016CW2/COMP3016CW2/OpenGLComp3016/Shape11.fbx");
    std::vector<Vertex> DAEVertices = loadDAEModel("H:/Comp3016/COMP3016CW2/COMP3016CW2/OpenGLComp3016/Signa.dae");


    std::vector<Vertex> modelVertices;

    // Append OBJ vertices to the combined modelVertices vector
    modelVertices.insert(modelVertices.end(), OBJVertices.begin(), OBJVertices.end());

    // Append FBX1 vertices to the combined modelVertices vector
    modelVertices.insert(modelVertices.end(), FBXVertices.begin(), FBXVertices.end());

    // Append DAE vertices to the combined modelVertices vector
    modelVertices.insert(modelVertices.end(), DAEVertices.begin(), DAEVertices.end());




    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, modelVertices.size() * sizeof(Vertex), modelVertices.data(), GL_STATIC_DRAW);

    // Specify the layout of the vertex data
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(1);


    // Specify the layout of the shape vertex data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);



    GLuint CubeVBO, CubeVAO;
    glGenVertexArrays(1, &CubeVAO);
    glGenBuffers(1, &CubeVBO);

    glBindVertexArray(CubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, CubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(shapeVertices), shapeVertices, GL_STATIC_DRAW);

    // Specify the layout of the vertex data
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Specify the layout of the shape vertex data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLuint vertexShader, fragmentShader;
    vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource, "VERTEX");
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource, "FRAGMENT");

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for shader program linking errors
    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(shaderProgram, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Shader program linking error: " << infoLog << std::endl;
        // Handle the error appropriately
    }


    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    float deltaTime = 0.0f;  // Time between current frame and last frame
    float lastFrame = 0.0f;   // Time of last frame
    float lightHeight = 1.0f; // Initial height of the light source

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        processInput(window, deltaTime);

        cameraFront = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f) *
            glm::mat3(glm::rotate(glm::mat4(1.0f), glm::radians(yaw), cameraUp) *
                glm::rotate(glm::mat4(1.0f), glm::radians(pitch), glm::cross(cameraFront, cameraUp))));

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear color and depth buffers

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        glUseProgram(shaderProgram);

        // Update light position
        float lightSpeed = 1.0f; // Adjust the speed of light movement
        lightHeight = 1.0f + sin(glfwGetTime() * lightSpeed) * 3.0f; // Adjust the amplitude of light movement
        glm::vec3 lightPos = glm::vec3(1.0f, lightHeight, 2.0f); // Set the light position

        glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, modelVertices.size());
        glBindVertexArray(0);

        glUseProgram(shaderProgram); // Use the same shader program as before

        // Adjust the translation values 
        glm::mat4 modelCube = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 0.0f, 0.0f));
        // Set the model matrix for the cube
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelCube));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glUseProgram(shaderProgram);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(shapeVertices) / (3 * sizeof(GLfloat)));
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }


    /*for (int i = 0; i < sizeof(vertices) / sizeof(vertices[0]); i += 3) {
        std::cout << vertices[i] << ", " << vertices[i + 1] << ", " << vertices[i + 2] << std::endl;
    }*/


    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);


    glfwTerminate();

    return 0;
}












