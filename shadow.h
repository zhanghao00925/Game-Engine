//
// Created by hao on 5/16/17.
//

#ifndef GAME_ENGINE_SHADOW_H
#define GAME_ENGINE_SHADOW_H

#include "definition.h"

class Shadow {
public:
    Shadow(unsigned int width, unsigned int height);
    void Bind();
    void Release();

    GLuint shadowTexId;
private:
    GLuint fbo;
    unsigned int shadowWidth, shadowHeight;
};


#endif //GAME_ENGINE_SHADOW_H
