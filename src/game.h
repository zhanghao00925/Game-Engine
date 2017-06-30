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

    void normalize() {
        if (nowRotate.x > 2 * PI)
            nowRotate.x = 0.0f;
        if (nowRotate.x < 0)
            nowRotate.x = 2.0f * PI;
        if (nowRotate.y > 2 * PI)
            nowRotate.y = 0.0f;
        if (nowRotate.y < 0)
            nowRotate.y = 2.0f * PI;
        if (nowRotate.z > 2 * PI)
            nowRotate.z = 0.0f;
        if (nowRotate.z < 0)
            nowRotate.z = 2.0f * PI;
    }
public:
    Game() {
        start = false;
        moving = true;
        touring = false;
        rotate = false;

        nowRotate.x = RANDOM_FLOAT * PI;
        nowRotate.y = RANDOM_FLOAT * PI;
        nowRotate.z = RANDOM_FLOAT * PI;

        winRotate.x = 0;
        winRotate.y = 0.5 * PI;
        winRotate.z = 0;
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
    bool isRotate() {
        return rotate;
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
    bool isTrue() {
        float diff = 0.0f;
        diff += abs(nowRotate.x - winRotate.x);
        diff += abs(nowRotate.y - winRotate.y);
        diff += abs(nowRotate.z - winRotate.z);
        if (diff < 0.3) {
            return true;
        } else {
            return false;
        }
    }
    void addXRotation(float angle) {
        nowRotate.x += angle / 50.0;
        normalize();
    }
    void addYRotation(float angle) {
        nowRotate.y += angle / 50.0;
        normalize();
    }
    void addZRotation(float angle) {
        nowRotate.z += angle / 50.0;
        normalize();
    }
    vec3 winRotate;
    vec3 nowRotate;
    bool rotate;
};


#endif //GAME_ENGINE_GAME_H
