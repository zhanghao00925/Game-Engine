//
// Created by hao on 5/16/17.
//

#include "skybox.h"

SkyBox::SkyBox(const vector<string> texturesPath) : cubemapTex(texturesPath) {}

void SkyBox::Draw(Shader &shader, Model model, mat4 &view, mat4 &projection) {
    glDepthMask(GL_FALSE);

    shader.Use();
    // Bind Texture
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shader.Program, "skyboxTex"), 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTex.texId);

    mat4 modelMatrix = mat4();
    model.Draw(shader, modelMatrix, view, projection);

    glDepthMask(GL_TRUE);
}