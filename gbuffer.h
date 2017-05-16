//
// Created by hao on 5/16/17.
//

#ifndef GAME_ENGINE_GBUFFER_H
#define GAME_ENGINE_GBUFFER_H

#include "definition.h"
#include "shader.h"

using namespace glm;

class GBuffer {
public:
    GBuffer();
    void Bind();
    void Release();
    void Draw(Shader &shader, vector<vec3> lightPositions, vector<vec3> lightColors, vec3 &cameraPostion);
    GLuint positionDepth, normal, albedo;
private:
    GLuint VAO, VBO, fbo;
};


#endif //GAME_ENGINE_GBUFFER_H
