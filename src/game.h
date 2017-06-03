//
// Created by hao on 6/2/17.
//

#ifndef GAME_ENGINE_GAME_H
#define GAME_ENGINE_GAME_H

#include "definition.h"

using namespace glm;

class Game {
public:
    unsigned int mission;

    bool start;
    bool moving;
    bool touring;
public:
    Game() {
        start = false;
        moving = true;
        touring = false;

        nowRotate.x = RANDOM_FLOAT * PI;
        nowRotate.y = RANDOM_FLOAT * PI;
        nowRotate.z = RANDOM_FLOAT * PI;
    }
    bool isStart() {
        return start;
    }
    bool isMoving() {
        return moving;
    }
    bool isTouring() {
        return touring;
    }
    void Cheat() {
        nowRotate = winRotate;
    }
    void Start() {
        start = true;
    }
    void Moved() {
        moving = false;
    }
    void Tour() {
        touring = true;
    }
    void isTrue() {

    }
    void addXRotation(float angle) {
        nowRotate.x += angle / 30.0;
    }
    void addYRotation(float angle) {
        nowRotate.y += angle / 30.0;
    }
    void addZRotation(float angle) {
        nowRotate.z += angle / 30.0;
    }

    vec3 winRotate;
    vec3 nowRotate;

};


#endif //GAME_ENGINE_GAME_H
