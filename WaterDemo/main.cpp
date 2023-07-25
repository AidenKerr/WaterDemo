#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <learnopengl/shader_s.h>
#include <learnopengl/camera.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void generateFlatPlane(float width, float depth, int divisions, std::vector<float> &vertices, std::vector<unsigned int> &indices);

const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

// set up camera
Camera camera(glm::vec3(0.0f, 0.5f, 10.0f));
float lastX = SCR_WIDTH / 2, lastY = SCR_HEIGHT / 2;
bool firstMouse = true;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

int main()
{
    std::cout << "Starting Water Demo" << std::endl;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Water Demo", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;

    }
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glEnable(GL_DEPTH_TEST);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    Shader shader("shaders/vertex.glsl", "shaders/fragment.glsl");

    // generate plane
    float width = 20.0f;
    float depth = 20.0f;
    int divisions = 50;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    generateFlatPlane(width, depth, divisions, vertices, indices);

    // Create VAO, VBO, and EBO
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind the VAO
    glBindVertexArray(VAO);

    // Bind and transfer vertex data to VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    // Bind and transfer index data to EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Set up vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Unbind VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // shader configuration
    shader.use();
    shader.setFloat("material.shininess", 64.0f);
    shader.setVec3("dirLight.direction", 0.0f, -0.2f, 1.0f);
    shader.setVec3("dirLight.ambient", 0.3f, 0.3f, 0.3f);
    shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

    while (!glfwWindowShouldClose(window))
    {
        // update deltaTime
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // process inputs
        processInput(window);

        // rendering
        glClearColor(0.2f, 0.6f, 0.75f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // update uniforms and matrices
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("proj", projection);
        shader.setVec3("viewPos", camera.Position);
        shader.setFloat("time", currentFrame);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, (GLsizei) indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }
    float xOffset = (float)xpos - lastX;
    float yOffset = lastY - (float)ypos;
    lastX = (float)xpos;
    lastY = (float)ypos;

    camera.ProcessMouseMovement(xOffset, yOffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll((float)yoffset);
}

void generateFlatPlane(float width, float depth, int divisions, std::vector<float>& vertices, std::vector<unsigned int>& indices) {
    float stepX = width / divisions;
    float stepZ = depth / divisions;
    int numVertices = (divisions + 1) * (divisions + 1);

    for (int i = 0; i <= divisions; i++) {
        for (int j = 0; j <= divisions; j++) {
            float x = -width / 2.0f + j * stepX;
            float z = -depth / 2.0f + i * stepZ;
            vertices.push_back(x);
            vertices.push_back(0.0f); // y-coordinate for a flat plane
            vertices.push_back(z);
        }
    }

    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            int row1 = i * (divisions + 1);
            int row2 = (i + 1) * (divisions + 1);
            indices.push_back(row1 + j);
            indices.push_back(row2 + j);
            indices.push_back(row1 + j + 1);
            indices.push_back(row1 + j + 1);
            indices.push_back(row2 + j);
            indices.push_back(row2 + j + 1);
        }
    }
}
