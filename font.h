//
// Created by hao on 5/16/17.
//

#ifndef GAME_ENGINE_FONT_H
#define GAME_ENGINE_FONT_H

#include "headers.h"
#include "shader.h"

using namespace std;

struct Character {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    GLuint Advance;    // Horizontal offset to advance to next glyph
};

class Font {
public:
    Font(const char *fontPath);
    void RenderText(Shader &shader, std::string text, GLfloat x, GLfloat y, GLfloat fontSize, glm::vec3 color);

    map<GLchar, Character> Characters;
    GLuint VAO, VBO;
};


#endif //GAME_ENGINE_FONT_H
