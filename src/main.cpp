#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

#include "../libs/gl/glad.h"
#include "../libs/gl/glfw3.h"
#include "../libs/glWrapper/glWrapper.hpp"
#include "../libs/tinygltf/stb_image.h"
#include "../libs/tinygltf/tinygltf.hpp"
#include "../libs/glm/glm.hpp"
#include "../libs/glm/gtc/matrix_transform.hpp"
#include "../libs/glm/gtc/type_ptr.hpp"

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float verts[]{
     //Location      //color        //coord
    -0.5, -0.5, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0,
    -0.5,  0.5, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
     0.5, -0.5, 0.0, 0.0, 0.0, 0.1, 1.0, 0.0,
     0.5,  0.5, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0
};

unsigned int inds[]{
    0, 1, 2,
    1, 2, 3
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    const float cameraSpeed = 2.5f * deltaTime; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) *
    cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) *
    cameraSpeed;

}

int main()
{
    if (!glfwInit())
    {
        std::cout << "Not working!\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(500, 500, "Test", NULL, NULL);
    if (!window)
    {
        std::cout << "No window\n";
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to load GLAD\n";
        glfwTerminate();
        return -1;
    }

    tinygltf::Model geometry;
    tinygltf::TinyGLTF loader;
    std::string error{};
    std::string warning{};

    // bool ret = loader.LoadBinaryFromFile(&geometry, &error, &warning, "assets/tower.glb");
    bool ret = loader.LoadASCIIFromFile(&geometry, &error, &warning, "assets/Cubes.gltf");

    std::cout << error << '\n' << warning << '\n' << ret << '\n';

    int accessor = geometry.meshes[0].primitives[0].attributes.at("POSITION");
    int accessor2 = geometry.meshes[0].primitives[0].indices;

    int bufferview = geometry.accessors[accessor].bufferView;
    int bufferview2 = geometry.accessors[accessor2].bufferView;

    int bytelength = geometry.bufferViews[bufferview].byteLength;

    std::cout << bufferview;

    glWrap::Texture2D texture1("assets/IdleMan.png", true, GL_NEAREST, GL_RGBA);

    glWrap::Shader shader("src/vertex.glsl", "src/fragment.glsl");

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, geometry.bufferViews[bufferview].byteLength, &geometry.buffers[0].data.at(0), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(inds), inds, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER ,0);
    glBindVertexArray(0);

    // std::cout << glGetError() << '\n';

    shader.Use();
    shader.SetInt("texture1", 0);

    glm::mat4 model{1.0f};
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.f / 600.f, 0.1f, 100.f);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        texture1.SetActive(0);

        float mixValue = sin(glfwGetTime());
        
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glm::mat4 view = glm::mat4(1.0);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
 
        shader.Use();

        shader.SetFloat("mix1", mixValue);

        shader.SetMatrix4("model", model);
        shader.SetMatrix4("view", view);
        shader.SetMatrix4("projection", projection);

        // unsigned int transLoc = glGetUniformLocation(shader.m_ID, "transform");
        // glUniformMatrix4fv(transLoc, 1, GL_FALSE, glm::value_ptr(trans));

        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}