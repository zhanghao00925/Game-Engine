#include "headers.h"
#include "definition.h"
#include "controller.h"
#include "model.h"
#include "skybox.h"
#include "screen.h"
#include "gbuffer.h"
#include "font.h"
#include "ssao.h"

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
    Camera *mainCamera = new Camera(vec3(10, 0, 0));
    Controller::BindCamera(mainCamera);
    // Setup skybox
    SkyBox skybox(skyBoxPath);
    Model skyboxModel("models/sphere.obj");
    Shader skyboxShader("shaders/skybox.vs", "shaders/skybox.frag");
    // Setup nanosuit
    Model nanosuitModel("models/nanosuit/nanosuit.obj");
    Shader modelShader("shaders/model.vs", "shaders/model.frag");
    // Setup screen
    Screen screen;
    Shader screenShader("shaders/screen.vs", "shaders/screenNormal.frag");
    // Setup Defer
    std::vector<glm::vec3> objectPositions;
    objectPositions.push_back(glm::vec3(-3.0, -3.0, -3.0));
    objectPositions.push_back(glm::vec3(0.0, -3.0, -3.0));
    objectPositions.push_back(glm::vec3(3.0, -3.0, -3.0));
    objectPositions.push_back(glm::vec3(-3.0, -3.0, 0.0));
    objectPositions.push_back(glm::vec3(0.0, -3.0, 0.0));
    objectPositions.push_back(glm::vec3(3.0, -3.0, 0.0));
    objectPositions.push_back(glm::vec3(-3.0, -3.0, 3.0));
    objectPositions.push_back(glm::vec3(0.0, -3.0, 3.0));
    objectPositions.push_back(glm::vec3(3.0, -3.0, 3.0));
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
    Shader deferGShader("shaders/deferG.vs", "shaders/deferG.frag");
    Shader deferLShader("shaders/deferL.vs", "shaders/deferL.frag");
    // Setup SSAO
    SSAO ssao;
    Shader ssaoGShader("shaders/ssaoG.vs", "shaders/ssaoG.frag");
    Shader ssaoShader("shaders/ssao.vs", "shaders/ssao.frag");
    Shader ssaoBlurShader("shaders/ssao.vs", "shaders/ssaoBlur.frag");
    Shader ssaoLShader("shaders/ssao.vs", "shaders/ssaoL.frag");
    // Setup text
    Font font("fonts/arial.ttf");
    Shader fontShader("shaders/font.vs", "shaders/font.frag");
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
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // We're not using stencil buffer now

        glm::mat4 model = glm::mat4();
        model = glm::translate(model, glm::vec3(0.0, -1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        glm::mat4 view = mainCamera->GetViewMatrix();
        glm::mat4 skyboxView = mat4(mat3(mainCamera->GetViewMatrix()));
        glm::mat4 projection = glm::perspective(mainCamera->Zoom, (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f);

        gBuffer.Bind();
        for (GLuint i = 0; i < objectPositions.size(); i++) {
            model = glm::mat4();
            model = glm::translate(model, objectPositions[i]);
            model = glm::scale(model, glm::vec3(0.25f));
            nanosuitModel.Draw(ssaoGShader, model, view, projection);
        }
        gBuffer.Release();

        ssao.SSAOProcess(ssaoShader, gBuffer, projection);
        ssao.SSAOBlurProcess(ssaoBlurShader);

        ssao.Draw(ssaoLShader, gBuffer, vec3(10, 10, 0), vec3(1, 1, 1), mainCamera->Position);

//        screen.Bind();
//        gBuffer.Draw(deferLShader, lightPositions, lightColors, mainCamera->Position);
//        screen.Release();
//
//        screen.Draw(screenShader);
//
//        font.Draw(fontShader, "SYSU Project", 25.0f, 25.0f, 0.5f, glm::vec3(0.5, 0.8f, 0.2f));

//         Swap the screen buffers
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
