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
#include "Shader.h"
#include "ModelLoader.h" 
using namespace std;

double lastX = 400.0;
double lastY = 300.0;
float yaw = 0.0f;
float pitch = 0.0f;
GLuint vertexShader, fragmentShader;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
bool firstMouse = true;
bool cursorLocked = true;



GLfloat shapeVertices[] = {
    // Cube vertices
    -0.5f, -0.5f, -0.5f,   // Bottom left
     0.5f, -0.5f, -0.5f,   // Bottom right
     0.5f, -0.5f,  0.5f,  // Top right

    -0.5f, -0.5f, -0.5f,  // Bottom left
     0.5f, -0.5f,  0.5f,   // Top right
    -0.5f, -0.5f,  0.5f,   // Top left

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
const float scaleFactor = 100.0f;

const char* vertexShaderSource = R"(
    #version 400 core
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
        // Transform vertex pos
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        // tranform to world space
        FragPos = vec3(model * vec4(aPos, 1.0));
        // Transform normals to world and pass to frag
        Normal = mat3(transpose(inverse(model))) * aNormal;
        //pass colour to frag
        Color = aColor;
    }
)";



const char* fragmentShaderSource = R"(
    #version 400 core
    in vec3 FragPos; //position from vertex shader
    in vec3 Normal; //normals from vertex
    in vec3 Color; //colour from vertex

    out vec4 FragColorOutput;

    uniform vec3 lightPos; // Position of first light
    uniform vec3 lightColor; // Color of first light
    uniform vec3 lightPos2; // Position of the second light
    uniform vec3 lightColor2; // Color of the second light
    uniform vec3 viewPos; 

    void main()
    {
        // Ambient lighting
        float ambientStrength = 0.3;
        vec3 ambient = ambientStrength * Color;

        // Diffuse lighting for the first light
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(Normal, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;

        // Diffuse lighting for the second light
        vec3 lightDir2 = normalize(lightPos2 - FragPos);
        float diff2 = max(dot(Normal, lightDir2), 0.0);
        vec3 diffuse2 = diff2 * lightColor2;

        // Specular lighting for the first light
        float specularStrength = 0.3;
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, Normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
        vec3 specular = specularStrength * spec * vec3(1.0, 1.0, 1.0);

        // Specular lighting for the second light
        vec3 reflectDir2 = reflect(-lightDir2, Normal);
        float spec2 = pow(max(dot(viewDir, reflectDir2), 0.0), 32.0);
        vec3 specular2 = specularStrength * spec2 * vec3(1.0, 1.0, 1.0);

         
        // Combine lighting contributions for both lights
        vec3 result = ambient + diffuse + specular + diffuse2 + specular2;
        FragColorOutput = vec4(result, 1.0);
    }
)";

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        cursorLocked = !cursorLocked;// toggle cursor lock

        if (cursorLocked) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        //initialise last mouse pos
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    // calc offset of mouse
    double xOffset = xpos - lastX;
    double yOffset = lastY - ypos;

    //update last mouse pos
    lastX = xpos;
    lastY = ypos;

    // sensitivity for mouse and scale offset by sens
    const float sensitivity = 0.05f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    // update yaw and pitch based on mouse movement
    yaw += static_cast<float>(xOffset);
    pitch += static_cast<float>(yOffset);

    // Clamp pitch to avoid flipping
    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    // Update camera front vector based on yaw and pitch
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}
void processInput(GLFWwindow* window, float deltaTime) {
    const float cameraSpeed = 2.5f * deltaTime;

    // Handle mouse input to change yaw and pitch
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    const float sensitivity = 0.05f;
    yaw += static_cast<float>(sensitivity * (mouseX - lastX));
    pitch += static_cast<float>(sensitivity * (lastY - mouseY));

    // Clamp pitch to avoid flipping
    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    lastX = mouseX;
    lastY = mouseY;

    // Update camera front vector
 
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);

    //calculate up and right camera vectors
    glm::vec3 right = glm::normalize(glm::cross(cameraFront, cameraUp));
    glm::vec3 up = glm::normalize(glm::cross(right, cameraFront));

    //keyboard inputs
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos += cameraSpeed * right;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos -= cameraSpeed * right;
    //keep camera at constant height
    cameraPos.y = 0.0f;
}

int main(void)
{
    //init GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    //create window
    GLFWwindow* window = glfwCreateWindow(1200, 800, "OpenGL Model Loading", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // set window settings
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    glEnable(GL_DEPTH_TEST);

    //init GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    // scale shapeVertices by scaleFactor
    for (int i = 0; i < sizeof(shapeVertices) / sizeof(shapeVertices[0]); ++i) {
        if (i % 3 == 0) {  // X-coordinate
            shapeVertices[i] *= scaleFactor;
        }
        else if (i % 3 == 1) {  // Y-coordinate
            shapeVertices[i] *= scaleFactor;
        }
        else if (i % 3 == 2) {  // Z-coordinate
            shapeVertices[i] *= scaleFactor;
        }
    }
    
    //load shaders and create shader program
    Shader shader(vertexShaderSource, fragmentShaderSource);
    shader.use();

    //load models using ModelLoader
    std::vector<Vertex> OBJVertices = ModelLoader::loadOBJModel("../lowpolysword1.obj");
    std::vector<Vertex> FBXVertices = ModelLoader::loadFBXModel("../Shape1.fbx");
    std::vector<Vertex> DAEVertices = ModelLoader::loadDAEModel("../Signa.dae");

    
    std::vector<Vertex> modelVertices;

    // combine vertices of all models
    modelVertices.insert(modelVertices.end(), OBJVertices.begin(), OBJVertices.end());
    modelVertices.insert(modelVertices.end(), FBXVertices.begin(), FBXVertices.end());
    modelVertices.insert(modelVertices.end(), DAEVertices.begin(), DAEVertices.end());

    //generate VAO and VBO for combined models vertices
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

    // same as before but for shapeVertices
    GLuint CubeVBO, CubeVAO;
    glGenVertexArrays(1, &CubeVAO);
    glGenBuffers(1, &CubeVBO);

    glBindVertexArray(CubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, CubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(shapeVertices), shapeVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //get shader ID and set shader
    GLuint shaderProgram = shader.getProgramID();

    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    float deltaTime = 0.0f;  // Time between current frame and last frame
    float lastFrame = 0.0f;   // Time of last frame
    float lightHeight = 1.0f; // Initial height of the light source
    float rotationSpeed = 15.0f; // Adjust the speed of rotation


    while (!glfwWindowShouldClose(window)) {
        //update time related variables
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // poll events and process input
        glfwPollEvents();
        processInput(window, deltaTime);
        float rotationAngle = currentFrame * rotationSpeed;

        cameraFront = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f) *
            glm::mat3(glm::rotate(glm::mat4(1.0f), glm::radians(yaw), cameraUp) *
                glm::rotate(glm::mat4(1.0f), glm::radians(pitch), glm::cross(cameraFront, cameraUp))));
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear color and depth buffers

        // applies rotation, defines camera view, camrea perspective
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        glUseProgram(shaderProgram);

        // Update light position
        float lightSpeed = 3.0f; // Adjust the speed of light movement
        lightHeight = 1.0f + sin(glfwGetTime() * lightSpeed) * 3.0f; // Adjust the amplitude of light movement
        // Update light positions and colors
        glm::vec3 lightPos = glm::vec3(1.0f, lightHeight, 2.0f);
        glm::vec3 lightColor = glm::vec3(0.5f, 0.5f, 0.5f); // White light

        float lightSpeed2 = 3.0f; // Adjust the speed of light2 movement
        float lightPos2X = 5.0f + cos(glfwGetTime() * lightSpeed2) * 3.0f; // Adjust the amplitude of light2 movement
        glm::vec3 lightPos2 = glm::vec3(lightPos2X, lightHeight, 1.0f);
        glm::vec3 lightColor2 = glm::vec3(0.0f, 0.08f, 0.0f); // Green light

        // Set light 1 properties
        glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), lightColor.x, lightColor.y, lightColor.z);

        // Set light 2 properties
        glUniform3f(glGetUniformLocation(shaderProgram, "lightPos2"), lightPos2.x, lightPos2.y, lightPos2.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor2"), lightColor2.x, lightColor2.y, lightColor2.z);
        // set for models
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        //render combined models
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, modelVertices.size());
        glBindVertexArray(0);

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
    //cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);


    glfwTerminate();

    return 0;
}












