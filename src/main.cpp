#include "headers.h"
#include "definition.h"
#include "controller.h"
#include "model.h"
#include "skybox.h"
#include "screen.h"
#include "gbuffer.h"
#include "font.h"
#include "ssao.h"
#include "water.h"
#include "particle.h"

int main() {
    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CG_DEMO", nullptr, nullptr); // Windowed
    glfwMakeContextCurrent(window);
    // Set the required callback functions
    glfwSetKeyCallback(window, Controller::KeyCallback);
    glfwSetCursorPosCallback(window, Controller::MouseCallback);
    // Options
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // Initialize GLEW to setup the OpenGL Function pointers
    glewExperimental = GL_TRUE;
    glewInit();
    // Define the viewport dimensions
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    // Setup some OpenGL options
    glEnable(GL_DEPTH_TEST);
    // Setup camera
    Camera *mainCamera = new Camera(vec3(0, 0, 10));
    Controller::BindCamera(mainCamera);
    // Setup skybox
    SkyBox skybox(skyBoxPath);
    Model skyboxModel("models/sphere.obj");
    Shader skyboxShader("shaders/skybox/skybox.vs", "shaders/skybox/skybox.frag");
    // Setup nanosuit
    Model nanosuitModel("models/nanosuit/nanosuit.obj");
    Shader modelShader("shaders/model/model.vs", "shaders/model/model.frag");
    // Setup screen
    Screen screen;
    Shader screenShader("shaders/screen/screen.vs", "shaders/screen/screenNormal.frag");
    // Setup Defer
    std::vector<glm::vec3> objectPositions;
    objectPositions.push_back(glm::vec3(-1.0, -1.0, -1.0));
    objectPositions.push_back(glm::vec3(0.0, -1.0, -1.0));
    objectPositions.push_back(glm::vec3(1.0, -1.0, -1.0));
    objectPositions.push_back(glm::vec3(-1.0, -1.0, 0.0));
    objectPositions.push_back(glm::vec3(0.0, -1.0, 0.0));
    objectPositions.push_back(glm::vec3(1.0, -1.0, 0.0));
    objectPositions.push_back(glm::vec3(-1.0, -1.0, 1.0));
    objectPositions.push_back(glm::vec3(0.0, -1.0, 1.0));
    objectPositions.push_back(glm::vec3(1.0, -1.0, 1.0));
    // - Colors
    const GLuint NR_LIGHTS = 32;
    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightColors;
    srand(13);
    for (GLuint i = 0; i < NR_LIGHTS; i++)
    {
        // Calculate slightly random offsets
        GLfloat xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
        GLfloat yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
        GLfloat zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
        lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
        // Also calculate random color
        GLfloat rColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
        GLfloat gColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
        GLfloat bColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
        lightColors.push_back(glm::vec3(rColor, gColor, bColor));
    }
    GBuffer gBuffer;
    Shader deferGShader("shaders/deferRender/deferG.vs", "shaders/deferRender/deferG.frag");
    Shader deferLShader("shaders/deferRender/deferL.vs", "shaders/deferRender/deferL.frag");
    // Setup SSAO
    SSAO ssao;
    Shader ssaoGShader("shaders/ssao/ssaoG.vs", "shaders/ssao/ssaoG.frag");
    Shader ssaoShader("shaders/ssao/ssao.vs", "shaders/ssao/ssao.frag");
    Shader ssaoBlurShader("shaders/ssao/ssao.vs", "shaders/ssao/ssaoBlur.frag");
    Shader ssaoLShader("shaders/ssao/ssao.vs", "shaders/ssao/ssaoL.frag");
    // Setup text
    Font font("fonts/arial.ttf");
    Shader fontShader("shaders/font/font.vs", "shaders/font/font.frag");
    // Setup water
    Texture woodTexture("textures/wood.png");
    Water water(woodTexture);
    water.Affect(50, 50);
    // Setup Particle System
    Texture particleTexture("textures/particle.png");
    Shader particleShader("shaders/particle/particle.vs", "shaders/particle/particle.frag");
    ParticleSystem particleSystem;
    particleSystem.Initialize(300);
    particleSystem.SetCreateSpeed(300, 0);
    particleSystem.recreateWhenDie = false;
    particleSystem.SetLifeTime(0.5, 1.5);
    particleSystem.SetCreationColor(vec3(1.0f,0.0f,0.0f), vec3(1.0f,0.5f,0.0f));
    particleSystem.SetDieColor(vec3(1.0f,1.0f,1.0f), vec3(1.0f,0.5f,0.0f));
    particleSystem.SetAlphaValues(1.0f,1.0f,0.3f,0.3f);
    particleSystem.SetEmitter(vec3(0.0f,0.0f,0.5f), vec3(0.1f,0.0f,0.1f));
    particleSystem.SetAcceleration(vec3(0.0f,1.0f,0.0f), 0.3f, 0.4f);
    particleSystem.SetSizeValues(0.04f,0.08f,0.06f,0.12f);
    particleSystem.SetEmitSpeed(0.2, 0.3);
    particleSystem.SetEmitDirection(vec3(0.0f,1.0f,0.0f), vec3(0.08f,0.5f,0.08f));
    particleSystem.SetTexture(particleTexture.texId);
    // Setup timer
    double deltaTime = 0.0f, lastFrame = 0.0f;
    // Game loop
    while (!glfwWindowShouldClose(window)) {

        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // Check and call events
        glfwPollEvents();
        Controller::Movement(deltaTime);
        // Render
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // We're not using stencil buffer now
        glm::mat4 model = glm::mat4();
        model = glm::translate(model, glm::vec3(0.0, -10.0f, 0.0f));
        glm::mat4 cameraView = mainCamera->GetViewMatrix();
        glm::mat4 cameraSkyboxView = mat4(mat3(cameraView));
        glm::mat4 waterView = water.GetViewMatrix(mainCamera->Front, mainCamera->Up);
        glm::mat4 waterSkyboxView = mat4(mat3(waterView));
        glm::mat4 projection = glm::perspective(mainCamera->Zoom, (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f);
//        // SSAO Test
//        gBuffer.Bind();
//        skybox.Draw(skyboxShader, skyboxModel, skyboxView, projection);
//        for (GLuint i = 0; i < objectPositions.size(); i++) {
//            model = glm::mat4();
//            model = glm::translate(model, objectPositions[i]);
//            model = glm::scale(model, glm::vec3(0.25f));
//            nanosuitModel.Draw(ssaoGShader, model, view, projection);
//        }
//        gBuffer.Release();
//        ssao.SSAOProcess(ssaoShader, gBuffer, projection);
//        ssao.SSAOBlurProcess(ssaoBlurShader);
//        ssao.Draw(ssaoLShader, gBuffer, vec3(10, 10, 0), vec3(1, 1, 1), mainCamera->Position);
//        // GBuffer Test
//        gBuffer.Bind();
//        for (GLuint i = 0; i < objectPositions.size(); i++) {
//            model = glm::mat4();
//            model = glm::translate(model, objectPositions[i]);
//            model = glm::scale(model, glm::vec3(0.25f));
//            nanosuitModel.Draw(deferGShader, model, view, projection);
//        }
//        gBuffer.Release();
//        gBuffer.Draw(deferLShader, lightPositions, lightColors, mainCamera->Position);
//        // Water Test
//        screen.Bind();
//        skybox.Draw(skyboxShader, skyboxModel, waterSkyboxView, projection);
//        nanosuitModel.Draw(modelShader, model, waterView, projection);
//        screen.Release();
//
//        skybox.Draw(skyboxShader, skyboxModel, cameraSkyboxView, projection);
//        water.ChangeTexture(screen.screenTexId);
//        water.Update(deltaTime);
//
//        water.Draw(modelShader, mat4(), cameraView, projection);
//        nanosuitModel.Draw(modelShader, model, cameraView, projection);
        particleSystem.Update(deltaTime);
        particleSystem.Draw(particleShader, cameraView, projection, mainCamera->Position);
//         Swap the screen buffers
        font.Draw(fontShader, "FPS: " + to_string(int(1 / deltaTime)), 700.0f, 570.0f, 0.5f, glm::vec3(0.5, 0.8f, 0.2f));
        glfwSwapBuffers(window);
    }

    delete mainCamera;
    glfwTerminate();
    return 0;
}
