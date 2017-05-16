//
// Created by hao on 5/16/17.
//

#ifndef GAME_ENGINE_SKYBOX_H
#define GAME_ENGINE_SKYBOX_H

#include "headers.h"
#include "definition.h"
#include "model.h"
#include "camera.h"

class SkyBox {
public:
    SkyBox(const vector<string> texturesPath);
    void Draw(Shader &shader, Model model, mat4 &view, mat4 &projection);

    Texture cubemapTex;
};



#endif //GAME_ENGINE_SKYBOX_H
