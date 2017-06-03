//
// Created by hao on 5/16/17.
//

#ifndef GAME_ENGINE_CONTROLLER_H
#define GAME_ENGINE_CONTROLLER_H

#include "headers.h"
#include "definition.h"
#include "camera.h"
#include "game.h"


class Controller {
public:
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void Movement(double deltaTime);
    static void BindCamera(Camera *camera);
    static void BindGame(Game *game);

private:
    static Camera *mainCamera;
    static Game *mainGame;
    static bool firstMouse;
    static bool keys[1024];
    static bool leftMouse, rightMouse, middleMouse;
    static double lastX, lastY;
};


#endif //GAME_ENGINE_CONTROLLER_H
