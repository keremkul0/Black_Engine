#include "Scene.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// Statik değişkenleri tanımla
unsigned int Scene::s_VAO = 0;
unsigned int Scene::s_VBO = 0;
bool Scene::s_Initialized = false;

// Küp vertex verileri (36 vertex)
static float cubeVertices[] = {
    // Positions        // Colors
    // Front face (red)
    -0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,

    // Back face (green)
    -0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 0.0f,

    // Left face (blue)
    -0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,   0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,

    // Right face (yellow)
     0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 0.0f,

    // Top face (magenta)
    -0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 1.0f,

    // Bottom face (cyan)
    -0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 1.0f
};
// Tek seferlik VAO-VBO oluştur
void Scene::InitCubeGeometry()
{
    if (s_Initialized) return;
    glGenVertexArrays(1, &s_VAO);
    glGenBuffers(1, &s_VBO);

    glBindVertexArray(s_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, s_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    s_Initialized = true;
}

void Scene::CleanUpCubeGeometry()
{
    if (s_Initialized)
    {
        glDeleteBuffers(1, &s_VBO);
        glDeleteVertexArrays(1, &s_VAO);
        s_VAO = 0;
        s_VBO = 0;
        s_Initialized = false;
        std::cout << "[Scene] Cube geometry cleaned up.\n";
    }
}

void Scene::AddCube(const glm::vec3& pos)
{
    Cube c;
    c.position = pos;
    c.rotation = glm::vec3(0.0f); // Initialize with no rotation
    cubes.push_back(c);
}

void Scene::UpdateCubeRotations(float currentTime)
{
    for (auto& cube : cubes)
    {
        // Different rotation speeds for each axis
        cube.rotation.x = currentTime * 0.5f;
        cube.rotation.y = currentTime * 0.7f;
        cube.rotation.z = currentTime * 0.3f;
    }
}
void Scene::DrawAllCubes(unsigned int shaderProgram, int width, int height)
{
    // Model-View-Projection matrices
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.f, -4.f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 100.0f);

    glUseProgram(shaderProgram);
    int mvpLoc = glGetUniformLocation(shaderProgram, "MVP");
    glBindVertexArray(s_VAO);

    // Draw each cube with rotation
    for (auto& cube : cubes)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cube.position);

        // Apply rotations on all three axes
        model = glm::rotate(model, cube.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, cube.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, cube.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

        glm::mat4 mvp = projection * view * model;
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glBindVertexArray(0);
    glUseProgram(0);
}
