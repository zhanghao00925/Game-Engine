//
// Created by hao on 5/16/17.
//

#include "shadow.h"

Shadow::Shadow(unsigned int width, unsigned int height)
        : shadowWidth(width), shadowHeight(height) {
    // Create the FBO
    glGenFramebuffers(1, &fbo);
    // Create the Texture
    glGenTextures(1, &shadowTexId);
    glBindTexture(GL_TEXTURE_2D, shadowTexId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // Bind to Depth Buffer
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTexId, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Shadow::Bind(Shader &shader) {
    shader.Use();
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
    // Set view
    glViewport(0, 0, shadowWidth, shadowHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void Shadow::Release() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Reset viewport
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
}

void Shadow::SetLightPosition(vec3 &lightPos) {
    this->lightPos = lightPos;
    glm::mat4 lightProjection, lightView;
//    lightProjection = glm::perspective(45.0f, (float)SCR_WIDTH/(float)SCR_HEIGHT, 1.0f, 50.0f);
    lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 1.0f, 60.0f);
    lightView = glm::lookAt(lightPos, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView;
}

void Shadow::Apply(Shader &shader, vec3 &viewPos) {
    shader.Use();
    glUniform3fv(glGetUniformLocation(shader.Program, "lightPos"), 1, &lightPos[0]);
    glUniform3fv(glGetUniformLocation(shader.Program, "viewPos"), 1, &viewPos[0]);
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
    // Set Texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, shadowTexId);
    glUniform1i(glGetUniformLocation(shader.Program, "shadowMap"), 1);
}