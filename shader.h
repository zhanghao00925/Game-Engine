//
// Created by hao on 5/16/17.
//

#ifndef GAME_ENGINE_SHADER_H
#define GAME_ENGINE_SHADER_H

#include "headers.h"

using namespace std;

class Shader {
public:
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
    void Use();

    GLuint Program;
};


#endif //GAME_ENGINE_SHADER_H
