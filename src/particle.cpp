//
// Created by hao on 5/18/17.
//

#include "particle.h"

#define RANDOM_FLOAT (((float)rand())/RAND_MAX)

GLfloat particleQuad[30] = {
        // Positions        // TexCoords
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
        1.0f, -1.0f, 0.0f,   1.0f, 0.0f,

        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
        1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,   1.0f, 1.0f
};

Particle::Particle() {
    isAlive = false;

}
void Particle::Initialize(ParticleSystem *belong) {
    dieAge = belong->minDieAge + (belong->maxDieAge - belong->minDieAge) * RANDOM_FLOAT;
    if (dieAge == 0.0f) {
        return;
    }
    age = 0.0f;
    // Init position
    position = belong->position;
    position.x += belong->maxCreationDev.x * (RANDOM_FLOAT*2.0f-1.0f);
    position.y += belong->maxCreationDev.y * (RANDOM_FLOAT*2.0f-1.0f);
    position.z += belong->maxCreationDev.z * (RANDOM_FLOAT*2.0f-1.0f);
    // Init velocity
    velocity = belong->direction;
    velocity.x += belong->maxDirectionDev.x * (RANDOM_FLOAT * 2.0f - 1.0f);
    velocity.y += belong->maxDirectionDev.y * (RANDOM_FLOAT * 2.0f - 1.0f);
    velocity.z += belong->maxDirectionDev.z * (RANDOM_FLOAT * 2.0f - 1.0f);
    velocity *= (belong->minEmitSpeed + (belong->maxEmitSpeed - belong->minEmitSpeed) * RANDOM_FLOAT);
    // Init acceleration
    acceleration = belong->accelerationDirection;
    acceleration *= (belong->minAcceleration + (belong->maxAcceleration - belong->minAcceleration) * RANDOM_FLOAT);
    // Init color
    color = belong->minEmitColor + (belong->maxEmitColor - belong->minEmitColor) * RANDOM_FLOAT;
    vec3 endColor = belong->minDieColor + (belong->maxDieColor - belong->minDieColor) * RANDOM_FLOAT;
    colorChange = (endColor - color) / dieAge;
    // Init alpha
    alpha = belong->minEmitAlpha + (belong->maxEmitAlpha - belong->minEmitAlpha) * RANDOM_FLOAT;
    float endAlpha = belong->minDieAlpha + (belong->maxDieAlpha - belong->minDieAlpha) * RANDOM_FLOAT;
    alphaChange = (endAlpha - alpha) / dieAge;
    // Init size
    size = belong->minEmitSize + (belong->maxEmitSize - belong->minEmitSize) * RANDOM_FLOAT;
    float endSize = belong->minDieSize + (belong->maxDieSize - belong->minDieSize) * RANDOM_FLOAT;
    sizeChange = (endSize - size) / dieAge;
    // Init spin
    spinAngle = 0.0f;
    spinSpeed = belong->minSpinSpeed + (belong->maxSpinSpeed - belong->minSpinSpeed) * RANDOM_FLOAT;
    spinAcceleration = belong->minSpinAcceleration + (belong->maxSpinAcceleration - belong->minSpinAcceleration) * RANDOM_FLOAT;
    // Finish
    this->belong = belong;
    this->isAlive = true;
}

void Particle::Update(float deltaTime) {
    age += deltaTime;
    if (age >= dieAge) {
        if (belong->recreateWhenDie) {
            Initialize(belong);
            Update(RANDOM_FLOAT * deltaTime);
        } else {
            age = 0.0;
            isAlive = false;
            belong->numOfParticlesInUse--;
        }
        return;
    }

    size += sizeChange * deltaTime;
    alpha += alphaChange * deltaTime;
    color += colorChange * deltaTime;
    velocity += acceleration * deltaTime;
    position += velocity * deltaTime;
    spinSpeed += spinAcceleration * deltaTime;
    spinAngle += spinSpeed * deltaTime;
}

void Particle::Draw(Shader &shader) {
    vec3 rotateX = belong->billBoardedX;
    vec3 rotateY = belong->billBoardedY;
    if (spinAngle > 0.0f)
    {
        rotateX = belong->billBoardedX * cos(spinAngle) + belong->billBoardedY * sin(spinAngle);
        rotateY = belong->billBoardedX * cos(spinAngle) - belong->billBoardedY * sin(spinAngle);
    }

    mat4 model;
    model = translate(model, position);
    model = scale(model, vec3(size, size, size));
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glUniform4f(glGetUniformLocation(shader.Program, "particleColor"), color.x, color.y, color.z, alpha);

    glBindVertexArray(belong->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

}

// ParticleSystem Part
ParticleSystem::ParticleSystem() {
    maxDieAlpha = 1.0f;
    minDieAlpha = 1.0f;

    minDieAge = 1.0f;
    maxDieAge = 1.0f;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // Bind VAO
    glBindVertexArray(VAO);
    // Bind VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particleQuad), particleQuad, GL_STATIC_DRAW);
    // Vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    // Vertex texture Coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    // Unbind
    glBindVertexArray(0);
}

void ParticleSystem::SetEmitter(vec3 position, vec3 deviation) {
    this->position = position;
    this->maxCreationDev = deviation;
}

void ParticleSystem::SetEmitDirection(vec3 direction, vec3 deviation) {
    this->direction = direction;
    this->maxDirectionDev = deviation;
}

void ParticleSystem::SetEmitSpeed(float minSpeed, float maxSpeed) {
    this->minEmitSpeed = minSpeed;
    this->maxEmitSize = maxSpeed;
}

void ParticleSystem::SetSpinSpeed(float min, float max) {
    minSpinSpeed = min;
    maxSpinSpeed = max;
}

void ParticleSystem::SetCreateSpeed(int createPerSec, float variance) {
    this->createPreSec = createPerSec;
    this->createVariance = variance;
}

void ParticleSystem::SetLifeTime(float minAge, float maxAge) {
    this->minDieAge = minAge;
    this->maxDieAge = maxAge;
}

void ParticleSystem::SetAcceleration(vec3 acceleration, float min, float max) {
    accelerationDirection = acceleration;
    minAcceleration = min;
    maxAcceleration = max;
}

void ParticleSystem::SetCreationColor(vec3 min, vec3 max) {
    minEmitColor = min;
    maxEmitColor = max;
}

void ParticleSystem::SetDieColor(vec3 min, vec3 max) {
    minDieColor = min;
    maxDieColor = max;
}

void ParticleSystem::SetAlphaValues(float minEmit, float maxEmit, float minDie, float maxDie) {
    minEmitAlpha = minEmit;
    maxEmitAlpha = maxEmit;
    minDieAlpha = minDie;
    maxDieAlpha = maxDie;
}

void ParticleSystem::SetSizeValues(float minEmit, float maxEmit, float minDie, float maxDie) {
    minEmitSize = minEmit;
    maxEmitSize = maxEmit;
    minDieSize = minDie;
    maxDieSize = maxDie;
}

void ParticleSystem::SetTexture(GLuint texId) {
    this->texId = texId;
}

void ParticleSystem::Initialize(int numOfParticles) {
    particles.resize(numOfParticles);
    maxParticles = numOfParticles;
    numOfParticlesInUse = 0;
}

void ParticleSystem::Update(float deltaTime) {
    int particlesToCreate = int(createPreSec * deltaTime * 10 * (1.0f + createVariance * (RANDOM_FLOAT - 0.5)));

    for (int i = 0; i < maxParticles; i++) {
        if (particles[i].isAlive) {
            particles[i].Update(deltaTime);
        }
        if (!particles[i].isAlive && particlesToCreate > 0) {
            particles[i].Initialize(this);
            particles[i].Update(RANDOM_FLOAT * deltaTime);
            particlesToCreate--;
            numOfParticlesInUse++;
        }
    }
}

void ParticleSystem::Draw(Shader &shader, mat4 view, mat4 projection) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    shader.Use();

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shader.Program, "texture_diffuse1"), 0);
    glBindTexture(GL_TEXTURE_2D, texId);

    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

//    billBoardedX = vec3(view[0], view[4], view[8]);
//    billBoardedY = vec3(view[1], view[5], view[9]);

    for (int i = 0; i < maxParticles; i++) {
        if (particles[i].isAlive) {
            particles[i].Draw(shader);
        }
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);

}