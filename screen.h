//
// Created by hao on 5/16/17.
//

#ifndef GAME_ENGINE_SCREEN_H
#define GAME_ENGINE_SCREEN_H

#include "headers.h"
#include "definition.h"
#include "shader.h"

using namespace std;

class Screen {
public:
    Screen();
    void Bind();
    void Release();
    void Draw(Shader &shader, GLfloat exposure = 1.0f);
    GLuint screenTexId;
private:
    GLuint fbo;
    GLuint VAO, VBO;
};


#endif //GAME_ENGINE_SCREEN_H
