//
// Created by hao on 5/16/17.
//

#ifndef GAME_ENGINE_MODEL_H
#define GAME_ENGINE_MODEL_H

#include "headers.h"
#include "shader.h"
#include "mesh.h"

using namespace std;

class Model {
public:
    Model(const string &path);
    void Draw(Shader &shader, mat4 &model, mat4 &view, mat4 &projection);
private:
    vector<Mesh> meshes;
    string directory;
    vector<Texture> texturesLoaded;

    void loadModel(const string &path);
    void processNode(aiNode* node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,const string &typeName);
};



#endif //GAME_ENGINE_MODEL_H
