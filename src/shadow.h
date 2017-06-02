//
// Created by hao on 5/16/17.
//

#ifndef GAME_ENGINE_SHADOW_H
#define GAME_ENGINE_SHADOW_H

#include "definition.h"
#include "shader.h"

using namespace glm;

class Shadow {
public:
    Shadow(unsigned int width, unsigned int height);
    void Bind(Shader &shadowShader);
    void Release();
    void Apply(Shader &shadowShader, vec3 &viewPos);
    void SetLightPosition(vec3 &lightPos);

    GLuint shadowTexId;
private:
    GLuint fbo;
    unsigned int shadowWidth, shadowHeight;
    vec3 lightPos;
    mat4 lightSpaceMatrix;
};


#endif //GAME_ENGINE_SHADOW_H
