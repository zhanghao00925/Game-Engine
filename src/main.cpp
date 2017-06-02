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
#include "object.h"
#include "shadow.h"

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
    glfwSetMouseButtonCallback(window, Controller::MouseButtonCallback);
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
    Camera *mainCamera = new Camera(vec3(-4.5, 13.0, 14.0));
    Controller::BindCamera(mainCamera);
    // Setup skybox
    SkyBox skybox(skyBoxPath);
    Model skyboxModel("models/sphere.obj");
    Shader skyboxShader("shaders/skybox/skybox.vs", "shaders/skybox/skybox.frag");
    // Setup nanosuit
    Model nanosuitModel("models/nanosuit/nanosuit.obj");
    Shader modelShader("shaders/model/model.vs", "shaders/model/model.frag");
    Shader shadowModelShader("shaders/model/shadowModel.vs", "shaders/model/shadowModel.frag");
    modelShader.Use();
    glUniform1f(glGetUniformLocation(modelShader.Program, "material.shininess"), 32.0f);
    glUniform3f(glGetUniformLocation(modelShader.Program, "light.ambient"),  0.2f, 0.2f, 0.2f);
    glUniform3f(glGetUniformLocation(modelShader.Program, "light.diffuse"),  0.5f, 0.5f, 0.5f);
    glUniform3f(glGetUniformLocation(modelShader.Program, "light.specular"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(modelShader.Program, "light.direction"), -1.0, -1.0, -1.0);
    // Setup Room
    Model room("models/Room/Room.obj");
    // Setup screen
    Screen screen;
    Shader screenShader("shaders/screen/screen.vs", "shaders/screen/screenNormal.frag");
    // Setup Defer
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
//    water.Affect(50, 50);
    // Setup Particle System
    Texture particleTexture("textures/particle1_.png");
    Shader particleShader("shaders/particle/particle.vs", "shaders/particle/particle.frag");
    ParticleSystem particleSystem;
    particleSystem.Initialize(300);
    particleSystem.SetCreateSpeed(300, 0);
    particleSystem.recreateWhenDie = false;
    particleSystem.SetLifeTime(2.0, 3.5);
    particleSystem.SetCreationColor(vec3(1.0f,0.0f,0.0f), vec3(1.0f,0.5f,0.0f));
    particleSystem.SetDieColor(vec3(1.0f,1.0f,1.0f), vec3(0.9f,0.4f,0.0f));
    particleSystem.SetAlphaValues(1.0f,1.0f,0.3f,0.3f);
    particleSystem.SetEmitter(vec3(0.0f,0.0f,0.5f), vec3(0.1f,0.0f,0.1f));
    particleSystem.SetAcceleration(vec3(0.0f,1.0f,0.0f), 0.3f, 0.4f);
    particleSystem.SetSizeValues(0.5f, 1.0f, 0.4f, 0.8f);
    particleSystem.SetEmitSpeed(0.2, 0.3);
    particleSystem.SetEmitDirection(vec3(0.0f,1.0f,0.0f), vec3(0.1f,0.5f,0.1f));
    particleSystem.SetTexture(particleTexture.texId);
    // Setup Shadow
    Shader shadowShader("shaders/shadow/shadow.vs", "shaders/shadow/shadow.frag");
    Shadow shadow(1024, 1024);
    // Setup Rock
    Model rockModel("models/rock/rock.obj");
    vec3 rockPosition(0, 100, 0);
    vec3 gravity(0.0f, 0.002f, 0.0f);
    vec3 velocity(0.0, 0.0, 0.0);
    // Setup Floor
    Object floor(Object_Type::PLANE);
    Object cube(Object_Type::CUBE);
    Object mirror(Object_Type::PLANE);

    vec3 lightPos = vec3(30, 5, 0);
    shadow.SetLightPosition(lightPos);
    vec3 firePositions[4] = {
            vec3(13.15f, 12.5f, -5.0f),
            vec3(-11.75f, 12.5f, -5.0f),
            vec3(3.4f, 5.3f, -5.7f),
            vec3(-3.2f, 5.3f, -5.7f)
    };
    // Setup Stencil
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    Shader stencilShader("shaders/stencil/stencil.vs", "shaders/stencil/stencil.frag");
    // Setup timer
    double deltaTime, lastFrame = 0.0f;
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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
//        cout << mainCamera->Position.x << " " << mainCamera->Position.y << " " << mainCamera->Position.z << endl;
        glStencilMask(0x00);
        glm::mat4 model;
        glm::mat4 cameraView = mainCamera->GetViewMatrix();
        glm::mat4 cameraSkyboxView = mat4(mat3(cameraView));
        glm::mat4 waterView = water.GetViewMatrix(vec3(0, 0, 0), mainCamera->Up);
        glm::mat4 waterSkyboxView = mat4(mat3(waterView));

        glm::mat4 mirrorView;
        glm::mat4 projection = glm::perspective(mainCamera->Zoom, (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 1000.0f);

        skybox.Draw(skyboxShader, skyboxModel, cameraSkyboxView, projection);
        // Set shadow
        shadow.Bind(shadowShader);
        // Room
        model = mat4();
        model = glm::scale(model, vec3(3.0f, 3.0f, 3.0f));
        room.Draw(shadowShader, model, cameraView, projection);
        // model 1
        model = mat4();
        model = glm::translate(mat4(), vec3(0, 5, 5));
        model = glm::scale(model, vec3(0.5, 0.5, 0.5));
        model = glm::rotate(model, (float)(-90.0f/180.0 * PI), vec3(0, 1, 0));
        nanosuitModel.Draw(shadowShader, model, cameraView, projection);
        // model 2
        model = mat4();
        model = glm::translate(mat4(), vec3(0, 5, 0));
        model = glm::scale(model, vec3(0.5, 0.5, 0.5));
        nanosuitModel.Draw(shadowShader, model, cameraView, projection);
        shadow.Release();
        shadow.Apply(shadowModelShader, mainCamera->Position);
        // Render room
        model = mat4();
        model = glm::scale(model, vec3(3.0f, 3.0f, 3.0f));
        room.Draw(shadowModelShader, model, cameraView, projection);
        // Setup Window
        screen.Bind();
        vec3 direction = vec3(-1.0f, 10.0f, 15.0f) + vec3(-1.0f, 10.0f, 15.0f) - mainCamera->Position;
        direction.y = 10.0f;
        mirrorView = lookAt(vec3(-1.0f, 10.0f, 15.0f), direction, vec3(0.0f, 1.0f, 0.0f));
        mirrorView = mat4(mat3(mirrorView));
        skybox.Draw(skyboxShader, skyboxModel, mirrorView, projection);
        screen.Release();
        // Render Window
        model = mat4();
        model = glm::translate(model, vec3(0, 10, -6));
        model = glm::rotate(model, (float)(90.0f/180.0f * PI), vec3(1, 0, 0));
        model = glm::translate(model, vec3(0, -5, 0));
        model = glm::scale(model, vec3(1, 0.5, 1));
        mirror.Draw(modelShader, model, cameraView, projection, screen.screenTexId);
        // Update four fire
        // Big fire
        particleSystem.SetSizeValues(0.5f, 1.0f, 0.4f, 0.8f);
        particleSystem.SetEmitter(vec3(13.15f, 12.5f, -5.0f), vec3(0.1f,0.0f,0.1f));
        particleSystem.Update(deltaTime);
        particleSystem.Draw(particleShader, cameraView, projection, mainCamera->Position);
        particleSystem.SetEmitter(vec3(-11.75f, 12.5f, -5.0f), vec3(0.1f,0.0f,0.1f));
        particleSystem.Update(deltaTime);
        particleSystem.Draw(particleShader, cameraView, projection, mainCamera->Position);
        // Small fire
        particleSystem.SetSizeValues(0.3f, 0.6f, 0.2f, 0.4f);
        particleSystem.SetEmitter(vec3(3.4f, 5.3f, -5.7f), vec3(0.1f,0.0f,0.1f));
        particleSystem.Update(deltaTime);
        particleSystem.Draw(particleShader, cameraView, projection, mainCamera->Position);
        particleSystem.SetEmitter(vec3(-3.2f, 5.3f, -5.7f), vec3(0.1f,0.0f,0.1f));
        particleSystem.Update(deltaTime);
        particleSystem.Draw(particleShader, cameraView, projection, mainCamera->Position);
        // Setup Water
        model = mat4();
        model = translate(model, vec3(-1.9, 4, 0.2));
        if (RANDOM_FLOAT < 0.2) {
            water.Affect(rand() % 80, rand() % 80);
        }
        water.Update(deltaTime);
        water.Draw(modelShader, model, cameraView, projection);
//      Swap the screen buffers
        font.Draw(fontShader, "FPS: " + to_string(int(1 / deltaTime)), 700.0f, 570.0f, 0.5f, glm::vec3(0.5, 0.8f, 0.2f));
        glfwSwapBuffers(window);
    }

    delete mainCamera;
    glfwTerminate();
    return 0;
}
