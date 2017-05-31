//
// Created by hao on 5/16/17.
//

#ifndef GAME_ENGINE_OBJECT_H
#define GAME_ENGINE_OBJECT_H

#include "headers.h"
#include "shader.h"

using namespace glm;

enum Object_Type {
    CUBE,
    PLANE
};
class Object {
public:
    Object(Object_Type type);
    void Draw(Shader shader, mat4 model, mat4 view, mat4 projection, GLuint &texId);
private:
    GLuint VBO, VAO;
    Object_Type type;
};


#endif //GAME_ENGINE_OBJECT_H
