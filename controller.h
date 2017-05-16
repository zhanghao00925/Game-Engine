//
// Created by hao on 5/16/17.
//

#ifndef GAME_ENGINE_CONTROLLER_H
#define GAME_ENGINE_CONTROLLER_H

#include "headers.h"
#include "definition.h"
#include "camera.h"


class Controller {
public:
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void Movement(double deltaTime);
    static void BindCamera(Camera *camera);

private:
    static Camera *mainCamera;
    static bool firstMouse;
    static bool keys[1024];
    static double lastX, lastY;
};


#endif //GAME_ENGINE_CONTROLLER_H
