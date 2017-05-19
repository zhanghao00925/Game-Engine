//
// Created by hao on 5/18/17.
//

#ifndef GAME_ENGINE_PARTICLE_H
#define GAME_ENGINE_PARTICLE_H

#include "definition.h"
#include "shader.h"

using namespace glm;

class ParticleSystem;

class Particle {
private:
    // Position Attrib
    vec3 position, velocity, acceleration;
    // Spin Attrib
    float spinAngle, spinSpeed, spinAcceleration;
    // Alpha and the speed it changes
    float alpha, alphaChange;
    // Color and the speed it changes
    vec3 color, colorChange;
    // Size and the speed it changes
    float size, sizeChange;
    // Age and its max lifeTime
    float age, dieAge;
    ParticleSystem *belong;
public:
    bool isAlive;
    Particle();
    void Initialize(ParticleSystem *belong);
    void Update(float deltaTime);
    void Draw(Shader &shader, float degree);
};


class ParticleSystem {
    friend class Particle;
public:
    ParticleSystem();
    void Initialize(int numOfParticles);
    void SetEmitter(vec3 position, vec3 deviation);
    void SetEmitSpeed(float minSpeed, float maxSpeed);
    void SetEmitDirection(vec3 direction, vec3 deviation);
    void SetSpinSpeed(float min, float max);
    void SetAcceleration(vec3 acceleration, float min, float max);
    void SetCreationColor(vec3 min, vec3 max);
    void SetDieColor(vec3 min, vec3 max);
    void SetAlphaValues(float minEmit, float maxEmit, float minDie, float maxDie);
    void SetSizeValues(float minEmit, float maxEmit, float minDie, float maxDie);
    void SetCreateSpeed(int createPerSec, float variance);
    void SetLifeTime(float minAge, float maxAge);
    void SetTexture(GLuint texId);
    void Update(float deltaTime);
    void Draw(Shader &shader, mat4 &view, mat4 &projection, vec3 &cameraPosition);
    bool recreateWhenDie;

private:
    // Emitter attrib
    // Create position and max offset
    vec3 position, maxCreationDev;
    // Emit direction and max offset
    vec3 direction, maxDirectionDev;
    // Emit speed
    float minEmitSpeed, maxEmitSpeed;
    vec3 accelerationDirection;
    float minAcceleration, maxAcceleration;
    // Spin speed
    float minSpinSpeed, maxSpinSpeed;
    float minSpinAcceleration, maxSpinAcceleration;
    // Particle Color
    float minEmitAlpha, maxEmitAlpha;
    float minDieAlpha, maxDieAlpha;
    // Particle Size
    float minEmitSize, maxEmitSize;
    float minDieSize, maxDieSize;
    // Particle Color
    vec3 minEmitColor, maxEmitColor;
    vec3 minDieColor, maxDieColor;
    // Particle Life
    float minDieAge, maxDieAge;
    // Render properties
    GLuint texId;
    // Particles attrib
    vector<Particle> particles;
    int maxParticles, numOfParticlesInUse;
    int createPreSec;
    float createVariance;
    vec3 billBoardedX, billBoardedY;
    GLuint VAO, VBO;
};

#endif //GAME_ENGINE_PARTICLE_H
