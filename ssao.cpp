//
// Created by hao on 5/16/17.
//

#include "ssao.h"

GLfloat ssaoQuad[24] = {
        // Positions   // TexCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
        1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
        1.0f, -1.0f,  1.0f, 0.0f,
        1.0f,  1.0f,  1.0f, 1.0f
};

SSAO::SSAO() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ssaoQuad), &ssaoQuad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
    glBindVertexArray(0);
    // Set SSAO Buffer
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    // Create Color Texture
    glGenTextures(1, &colorTexId);
        glBindTexture(GL_TEXTURE_2D, colorTexId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexId, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "SSAO Framebuffer not complete!" << std::endl;
    // Set Blur Buffer
    glGenFramebuffers(1, &blurfbo);
    glBindFramebuffer(GL_FRAMEBUFFER, blurfbo);
    // Create Color Blur Texture
    glGenTextures(1, &colorBlurTexId);
        glBindTexture(GL_TEXTURE_2D, colorBlurTexId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBlurTexId, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
        // Reset Frame Buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Sample kernel
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    for (GLuint i = 0; i < 64; ++i) {
        // X:(-1, 1) Y:(-1, 1) Z(0, 1)
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        // Init direction
        sample = glm::normalize(sample);
        // Init distance
        sample *= randomFloats(generator);
        GLfloat scale = GLfloat(i) / 64.0;
        // Scale samples s.t. they're more aligned to center of kernel
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }
    // Create Noise texture
    for (GLuint i = 0; i < 16; i++) {
        // X:(-1, 1) Y:(-1, 1) Z:(0) rotate along Z-axis
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }
    glGenTextures(1, &noiseTexId);
        glBindTexture(GL_TEXTURE_2D, noiseTexId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void SSAO::SSAOProcess(Shader &shader, GBuffer &gBuffer, mat4 &projection) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_COLOR_BUFFER_BIT);
    shader.Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gBuffer.positionDepth);
    glUniform1i(glGetUniformLocation(shader.Program, "gPositionDepth"), 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gBuffer.normal);
    glUniform1i(glGetUniformLocation(shader.Program, "gNormal"), 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, noiseTexId);
    glUniform1i(glGetUniformLocation(shader.Program, "texNoise"), 2);
    // Send kernel + rotation
    for (GLuint i = 0; i < 64; ++i)
        glUniform3fv(glGetUniformLocation(shader.Program, ("samples[" + std::to_string(i) + "]").c_str()), 1, &ssaoKernel[i][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void SSAO::SSAOBlurProcess(Shader &shader) {
    glBindFramebuffer(GL_FRAMEBUFFER, blurfbo);
    glClear(GL_COLOR_BUFFER_BIT);
    shader.Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorTexId);
    glUniform1i(glGetUniformLocation(shader.Program, "ssaoInput"), 0);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAO::Draw(Shader &shader, GBuffer &gBuffer, vec3 lightPosition, vec3 lightColor, vec3 &cameraPostion) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader.Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gBuffer.positionDepth);
    glUniform1i(glGetUniformLocation(shader.Program, "gPositionDepth"), 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gBuffer.normal);
    glUniform1i(glGetUniformLocation(shader.Program, "gNormal"), 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gBuffer.albedo);
    glUniform1i(glGetUniformLocation(shader.Program, "gAlbedo"), 2);
    glActiveTexture(GL_TEXTURE3); // Add extra SSAO texture to lighting pass
    glBindTexture(GL_TEXTURE_2D, colorBlurTexId);
    glUniform1i(glGetUniformLocation(shader.Program, "ssao"), 3);
    // Also send light relevant uniforms
    glUniform3fv(glGetUniformLocation(shader.Program, "light.Position"), 1, &lightPosition[0]);
    glUniform3fv(glGetUniformLocation(shader.Program, "light.Color"), 1, &lightColor[0]);
    // Update attenuation parameters
    const GLfloat constant = 1.0; // Note that we don't send this to the shader, we assume it is always 1.0 (in our case)
    const GLfloat linear = 0.7;
    const GLfloat quadratic = 1.8;
    glUniform1f(glGetUniformLocation(shader.Program, "light.Linear"), linear);
    glUniform1f(glGetUniformLocation(shader.Program, "light.Quadratic"), quadratic);
    glUniform3fv(glGetUniformLocation(shader.Program, "viewPos"), 1, &cameraPostion[0]);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

GLfloat SSAO::lerp(GLfloat a, GLfloat b, GLfloat f) {
    return a + f * (b - a);
}