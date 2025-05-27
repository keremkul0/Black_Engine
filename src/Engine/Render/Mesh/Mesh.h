#ifndef MESH_H
#define MESH_H

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "VBO/VBO.h"
#include "EBO/EBO.h"
#include "VAO/VAO.h"



class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    VAO VAO;;


    /////////////////////////
    Mesh() = default; // Added default constructor
    ~Mesh();

    void Initialize(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices);

    void Draw();


private:

    /////////////////////////
    //GLuint VAO = 0;
};

#endif // MESH_H
