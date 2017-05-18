//
// Created by hao on 5/18/17.
//

#include "water.h"

Water::Water(Texture texture, int xSize, int zSize, float offset, float weight, float damping) {
    this->xSize = xSize;
    this->zSize = zSize;
    this->offset = offset;
    this->weight = weight;
    this->damping = damping;
    this->texId = texture.texId;
    oscillators.resize(xSize * zSize);
    for (int x = 0; x < xSize; x++) {
        for (int z = 0; z < zSize; z++) {
            int cursor = x + z * xSize;
            // Set position;
            oscillators[cursor].position.x = offset * float(x);
            oscillators[cursor].position.y = 0.0f;
            oscillators[cursor].position.z = offset * float(z);
            // Set normal;
            oscillators[cursor].normal.x = 0.0f;
            oscillators[cursor].normal.y = 1.0f;
            oscillators[cursor].normal.z = 0.0f;
            // Set texture coords
            oscillators[cursor].texCoords.x = (float)x / (float)xSize;
            oscillators[cursor].texCoords.y = 1.0f - (float)z / (float)zSize;
            // Set other attribs
            oscillators[cursor].upSpeed = 0;
            // Add index
            if ((x < xSize - 1) && (z < zSize - 1)) {
                indices.push_back(x + z * xSize);
                indices.push_back((x + 1) + z * xSize);
                indices.push_back((x + 1) + (z + 1) * xSize);

                indices.push_back(x + z * xSize);
                indices.push_back((x + 1) + (z + 1) * xSize);
                indices.push_back(x + (z + 1) * xSize);
            }
        }
    }
    // Create
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // Bind VAO
    glBindVertexArray(VAO);
    // Bind VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, oscillators.size() * sizeof(Oscillator), &oscillators[0], GL_DYNAMIC_DRAW);
    // Bind EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
    // Vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Oscillator), (GLvoid*)0);
    // Vertex Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Oscillator), (GLvoid*)offsetof(Oscillator, normal));
    // Vertex texture Coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Oscillator), (GLvoid*)offsetof(Oscillator, texCoords));
    // Unbind
    glBindVertexArray(0);
}

void Water::Reset() {
    for (int x = 0; x < xSize; x++) {
        for (int z = 0; z < zSize; z++) {
            int cursor = x + z * xSize;
            oscillators[cursor].position.y = 0.0f;
            oscillators[cursor].upSpeed = 0.0f;
            oscillators[cursor].normal.x = 0.0f;
            oscillators[cursor].normal.y = 1.0f;
            oscillators[cursor].normal.z = 0.0f;
        }
    }
}

void Water::Affect(int xPos, int zPos, float deltaY) {
    if ((xPos >= 0) && (xPos < xSize) && (zPos >= 0) && (zPos < zSize)) {
        if (oscillators[xPos + zPos * xSize].position.y > -0.15)  //THIS LINE IS REQUIRED FOR FOUNTAINS WITH MANY DROPS!!!
            oscillators[xPos + zPos * xSize].position.y += deltaY;
    }
}

void Water::Update(float deltaTime) {
    int x, z;
    for (x = 0; x < xSize; x++) {
        for (z = 0; z < zSize; z++) {
            int cursor = x + z * xSize;
            oscillators[cursor].newY = oscillators[cursor].position.y;
            if (x == 0 || x == xSize - 1 || z == 0 || z == zSize - 1) {

            } else {
                float avgDiff = oscillators[cursor - 1].position.y
                                + oscillators[cursor + 1].position.y
                                + oscillators[cursor - xSize].position.y
                                + oscillators[cursor + xSize].position.y
                                - 4 * oscillators[cursor].position.y;
                oscillators[cursor].upSpeed += avgDiff * (deltaTime / weight);
                oscillators[cursor].upSpeed *= (1.0 - damping);

                oscillators[cursor].newY += oscillators[cursor].upSpeed * deltaTime;
            }
        }
    }
    // Replace y position
    for (x = 0; x < xSize; x++) {
        for (z = 0; z < zSize; z++) {
            oscillators[x + z * xSize].position.y = oscillators[x + z * xSize].newY;
        }
    }
    // Calculator Normal
    for (x = 0; x < xSize; x++) {
        for (z = 0; z < zSize; z++) {
            vec3 u, v, p1, p2;
            if (x > 0)
                p1 = oscillators[x - 1 + z * xSize].position;
            else
                p1 = oscillators[x + z * xSize].position;
            if (x < xSize - 1)
                p2 = oscillators[x + 1 + z * xSize].position;
            else
                p2 = oscillators[x + z * xSize].position;
            u = p2 - p1;
            //vector from the left neighbor to the right neighbor
            if (z > 0)
                p1 = oscillators[x + (z - 1) * xSize].position;
            else
                p1 = oscillators[x + z * xSize].position;
            if (z < zSize - 1)
                p2 = oscillators[x + (z + 1)  * xSize].position;
            else
                p2 = oscillators[x + z * xSize].position;
            v = p2 - p1;
            //vector from the upper neighbor to the lower neighbor
            vec3 normal = glm::normalize(cross(u, v));
            if (normal.y > 0.0) {
                oscillators[x + z * xSize].normal = normal;
            } else {
                oscillators[x + z * xSize].normal = -normal;
            }
        }
    }
}

void Water::ChangeTexture(GLuint textureID) {
    this->texId = textureID;
}

mat4 Water::GetViewMatrix(vec3 front, vec3 up) {
    return glm::lookAt(vec3(offset * xSize / 2, 2, offset * zSize / 2),
                       vec3(offset * xSize / 2, 0, offset * zSize / 2) + front,
                       up);
}

void Water::Draw(Shader shader, mat4 model, mat4 view, mat4 projection) {

    shader.Use();

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shader.Program, "texture_diffuse1"), 0);
    glBindTexture(GL_TEXTURE_2D, texId);
    glActiveTexture(GL_TEXTURE1);
    glUniform1i(glGetUniformLocation(shader.Program, "texture_specular1"), 1);
    glBindTexture(GL_TEXTURE_2D, texId);

    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, oscillators.size() * sizeof(Oscillator), &oscillators[0]);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}