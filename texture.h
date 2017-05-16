//
// Created by hao on 5/16/17.
//

#ifndef GAME_ENGINE_TEXTURE_H
#define GAME_ENGINE_TEXTURE_H

#include "headers.h"

using namespace std;

class Texture {
public:
    Texture(const string &Filename);
    Texture(const vector<string> &texturesPath);
    string type;
    aiString path;
    GLuint texId;
};


#endif //GAME_ENGINE_TEXTURE_H
