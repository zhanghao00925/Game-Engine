//
// Created by hao on 5/18/17.
//

#ifndef GAME_ENGINE_WATER_H
#define GAME_ENGINE_WATER_H

#include "definition.h"
#include "shader.h"
#include "texture.h"

using namespace glm;

struct Oscillator {
    vec3 position;
    vec3 normal;
    vec2 texCoords;

    GLfloat upSpeed;
    GLfloat newY;
};

class Water {
public:
    Water(Texture texture, int xSize = 150, int zSize = 150, float offset = 0.05, float weight = 0.01, float damping = 0.002);
    void Reset();
    void Affect(int xPos, int zPos, float deltaY = -0.05d);
    void ChangeTexture(GLuint textureID);
    mat4 GetViewMatrix(vec3 front, vec3 up);
    void Update(float deltaTima);
    void Draw(Shader shader, mat4 model, mat4 view, mat4 projection);
private:
    vector<Oscillator> oscillators;
    vector<GLuint> indices;
    GLuint VAO, VBO, EBO, texId;
    GLuint fbo, envTexId;
    int xSize, zSize;
    float offset;
    float weight;
    float damping;
};


#endif //GAME_ENGINE_WATER_H
