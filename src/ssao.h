//
// Created by hao on 5/16/17.
//

#ifndef GAME_ENGINE_SSAO_H
#define GAME_ENGINE_SSAO_H

#include "definition.h"
#include "shader.h"
#include "gbuffer.h"

class SSAO {
public:
    SSAO();
    void SSAOProcess(Shader &shader, GBuffer &gBuffer, mat4 &projection);
    void SSAOBlurProcess(Shader &shader);
    void Draw(Shader &shader, GBuffer &gBuffer, vec3 lightPosition, vec3 lightColor, vec3 &cameraPostion);
    GLuint colorTexId, colorBlurTexId, noiseTexId;
private:
    GLuint fbo, blurfbo;
    GLuint VAO, VBO;
    std::vector<glm::vec3> ssaoKernel;
    std::vector<glm::vec3> ssaoNoise;
    GLfloat lerp(GLfloat a, GLfloat b, GLfloat f);
};

#endif //GAME_ENGINE_SSAO_H
