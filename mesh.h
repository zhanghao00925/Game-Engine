//
// Created by hao on 5/16/17.
//

#ifndef GAME_ENGINE_MESH_H
#define GAME_ENGINE_MESH_H

#include "headers.h"
#include "texture.h"
#include "shader.h"

using namespace glm;
using namespace std;

struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 texCoords;

    Vertex() = default;

    Vertex(const vec3 &position, const vec3 &normal, const vec2 &texCoords) {
        this->position = position;
        this->normal = normal;
        this->texCoords = texCoords;
    }
};

class Mesh {
public:
    vector<Vertex> vertices;
    vector<GLuint> indices;
    vector<Texture> textures;

    Mesh(const vector<Vertex> &vertices,
         const vector<GLuint> &indices,
         const vector<Texture> &textures);
    void Draw(Shader &shader);
private:
    GLuint VAO, VBO, EBO;
    void setupMesh();
};


#endif //GAME_ENGINE_MESH_H
