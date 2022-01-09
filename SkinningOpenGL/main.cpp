#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include"Scene.h"
#include "shader.h"
using namespace std;

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;
void initGL() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    /*glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);*/
}

int main() {
    initGL();
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Skinning", nullptr, nullptr);

    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return 0;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGL()) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return 0 ;
    }
    /*std::cout << glGetString(GL_VERSION) << std::endl;*/

    /*stbi_set_flip_vertically_on_load(true);*/


    glEnable(GL_DEPTH_TEST);


    string filePath = "./jumpdown/jump_down.dae";
    Shader shader("skinning.vert", "skinning.frag");
    Scene scene(filePath);

    float lastTime = 0;
    while (!glfwWindowShouldClose(window))
    {
        float currTime = glfwGetTime();
        float interval = currTime - lastTime;
        lastTime = currTime;

        scene.update(interval);
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();

        glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)WIDTH/ (float)HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.7f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f));
        glm::mat4 mvpMat = proj * view * model;

        shader.setMat4("mvpMat", mvpMat);

        auto size = scene.finalBoneTransformations.size();
        for (int i = 0; i < size; i++) {
            shader.setMat4("boneTrans[" + std::to_string(i) + "]", scene.finalBoneTransformations[i]);
        }

        scene.Draw(shader);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}


