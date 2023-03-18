#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

#include "../libs/gl/glad.h"
#include "../libs/gl/glfw3.h"
#include "../libs/glWrapper/glWrapper.hpp"
#include "../libs/stb/stb_image.h"

float verts[]{
     //Location      //color        //coord
    -1.0, -1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0,
    -1.0,  1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
     1.0, -1.0, 0.0, 0.0, 0.0, 0.1, 1.0, 0.0,
     1.0,  1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0
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

    glWrap::Texture2D texture1("assets/IdleMan.png", true, GL_NEAREST, GL_RGBA);
    glWrap::Texture2D texture2("assets/RunningMan.png", true, GL_NEAREST, GL_RGBA);


    glWrap::Shader shader("src/vertex.glsl", "src/fragment.glsl");

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(inds), inds, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER ,0);
    glBindVertexArray(0);

    // std::cout << glGetError() << '\n';

    shader.Use();
    shader.SetInt("texture1", 0);
    shader.SetInt("texture2", 1);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        texture1.SetActive(0);
        texture2.SetActive(1);

        float mixValue = sin(glfwGetTime());

        shader.Use();

        shader.SetFloat("mix1", mixValue);

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