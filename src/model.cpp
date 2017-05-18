//
// Created by hao on 5/16/17.
//

#include "model.h"

Model::Model(const string &path) {
    loadModel(path);
}

void Model::Draw(Shader &shader, mat4 &model, mat4 &view, mat4 &projection) {
    shader.Use();

    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    for (unsigned i = 0; i < meshes.size(); i++) {
        meshes[i].Draw(shader);
    }
}

void Model::loadModel(const string &path) {
    // Get scene
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        cout << "ERROR:MODEL::LOADMODEL: "<< importer.GetErrorString();
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
    aiMesh *mesh = nullptr;
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    vector<Vertex> vertices;
    // Process mesh vertices
    vec3 position, normal;
    vec2 texcoords;
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        // Set texture
        position.x = mesh->mVertices[i].x;
        position.y = mesh->mVertices[i].y;
        position.z = mesh->mVertices[i].z;
        // Set normal
        normal.x = mesh->mNormals[i].x;
        normal.y = mesh->mNormals[i].y;
        normal.z = mesh->mNormals[i].z;
        // Set texcoords
        if (mesh->mTextureCoords[0]) {
            texcoords.x = mesh->mTextureCoords[0][i].x;
            texcoords.y = mesh->mTextureCoords[0][i].y;
        } else {
            texcoords = vec2(0, 0);
        }
        vertices.push_back(Vertex(position, normal, texcoords));
    }
    vector<GLuint> indices;
    aiFace face;
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        face = mesh->mFaces[i];
        // push all triangle
        for (unsigned j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }
    vector<Texture> textures;
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        // 1. Diffuse maps
        vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. Specular maps
        vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return Mesh(vertices, indices, textures);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type,const string &typeName) {
    vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for (unsigned j = 0; j < texturesLoaded.size(); j++) {
            if (strcmp(texturesLoaded[j].path.C_Str(), str.C_Str()) == 0) {
                textures.push_back(texturesLoaded[j]);
                skip = true;
                // A texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if (!skip) {
            Texture texture(directory + "/" + str.C_Str());
            texture.type = typeName;
            texture.path = str;
            textures.push_back(texture);
            texturesLoaded.push_back(texture);
        }
    }
    return textures;
}
