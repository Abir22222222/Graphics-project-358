#include "glad.h"
#include "glfw3.h"

#include <iostream>
#include <cmath>
#include <vector>
#include <cstdio>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void checkShaderCompile(unsigned int shader, std::string type);
void setupCircleVAO(unsigned int &vao, unsigned int &vbo, float radius, int segments);

const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 700;


float playerX = 0;
float ballX = 0;
float ballY = -55;
float velocityX = 0;
float velocityY = 0;
bool shooting = false;
int score = 0;

// Vertex Shader
const char *vertexShaderSource =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform vec2 translation;\n"
"void main()\n"
"{\n"
"    // Ortho projection matrix 2D manually calculated for (-100, 100, -100, 100)\n"
"    vec2 projectedPos = (aPos.xy + translation) / 100.0;\n"
"    gl_Position = vec4(projectedPos, 0.0, 1.0);\n"
"}\0";

// Fragment Shader
const char *fragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec3 color;\n"
"void main()\n"
"{\n"
"    FragColor = vec4(color, 1.0);\n"
"}\0";


void updateBall() {
    if (shooting) {
        ballX += velocityX;
        ballY += velocityY;
        velocityY -= 0.1;

        // Score detect
        if (ballX > 55 && ballX < 65 && ballY > 13 && ballY < 17) {
            score++;
            shooting = false;
            ballX = playerX;
            ballY = -55;
        }

        // Ground hit
        if (ballY < -75) {
            shooting = false;
            ballX = playerX;
            ballY = -55;
        }
    }
    else {
        ballX = playerX;
    }
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Basketball Game", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback); // কীবোর্ড হ্যান্ডলার

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Shader setup
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    checkShaderCompile(vertexShader, "VERTEX");

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    checkShaderCompile(fragmentShader, "FRAGMENT");

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkShaderCompile(shaderProgram, "PROGRAM");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Uniform location casing
    int translationLoc = glGetUniformLocation(shaderProgram, "translation");
    int colorLoc = glGetUniformLocation(shaderProgram, "color");

    // --- . Ground VAO/VBO ---
    float groundVertices[] = {
        -100.0f, -80.0f, 0.0f,
         100.0f, -80.0f, 0.0f,
         100.0f, -100.0f, 0.0f,
        -100.0f, -100.0f, 0.0f
    };
    unsigned int groundIndices[] = { 0, 1, 2, 0, 2, 3 };
    unsigned int groundVAO, groundVBO, groundEBO;
    glGenVertexArrays(1, &groundVAO);
    glGenBuffers(1, &groundVBO);
    glGenBuffers(1, &groundEBO);
    glBindVertexArray(groundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, groundEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(groundIndices), groundIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // --- . Basket Pole, Board, Ring VAO/VBO ---
    float basketVertices[] = {
        // Pole Lines (Indices 0, 1)
        70.0f, -80.0f, 0.0f,
        70.0f,  30.0f, 0.0f,
        // Board Quads (Indices 2, 3, 4, 5)
        55.0f,  20.0f, 0.0f,
        75.0f,  20.0f, 0.0f,
        75.0f,  35.0f, 0.0f,
        55.0f,  35.0f, 0.0f,
        // Ring Lines (Indices 6, 7)
        55.0f,  15.0f, 0.0f,
        65.0f,  15.0f, 0.0f
    };
    unsigned int boardIndices[] = { 2, 3, 4, 2, 4, 5 };

    unsigned int basketVAO, basketVBO, boardEBO;
    glGenVertexArrays(1, &basketVAO);
    glGenBuffers(1, &basketVBO);
    glGenBuffers(1, &boardEBO);
    glBindVertexArray(basketVAO);
    glBindBuffer(GL_ARRAY_BUFFER, basketVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(basketVertices), basketVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boardEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(boardIndices), boardIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // --- . Player Body (Lines) VAO/VBO ---
    float playerLinesVertices[] = {
        0.0f, -45.0f, 0.0f,  0.0f, -65.0f, 0.0f, // Spine
       -8.0f, -50.0f, 0.0f,  8.0f, -50.0f, 0.0f, // Arms
        0.0f, -65.0f, 0.0f, -5.0f, -75.0f, 0.0f, // Leg 1
        0.0f, -65.0f, 0.0f,  5.0f, -75.0f, 0.0f  // Leg 2
    };
    unsigned int playerLinesVAO, playerLinesVBO;
    glGenVertexArrays(1, &playerLinesVAO);
    glGenBuffers(1, &playerLinesVBO);
    glBindVertexArray(playerLinesVAO);
    glBindBuffer(GL_ARRAY_BUFFER, playerLinesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(playerLinesVertices), playerLinesVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // --- . Circles (Head & Basketball) VAO/VBO জেনারেশন ---
    unsigned int headVAO, headVBO;
    setupCircleVAO(headVAO, headVBO, 5.0f, 360); // Head radius 5

    unsigned int ballVAO, ballVBO;
    setupCircleVAO(ballVAO, ballVBO, 4.0f, 360); // Ball radius 4

    glBindVertexArray(0);

    double lastTime = glfwGetTime();

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

    
        double currentTime = glfwGetTime();
        if (currentTime - lastTime >= 0.016) {
            updateBall();
            lastTime = currentTime;
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        //  Draw Ground
        glUniform2f(translationLoc, 0.0f, 0.0f);
        glUniform3f(colorLoc, 0.8f, 0.5f, 0.2f);
        glBindVertexArray(groundVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // . Draw Player Head
        glUniform2f(translationLoc, playerX, -40.0f);
        glUniform3f(colorLoc, 1.0f, 0.8f, 0.6f);
        glBindVertexArray(headVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 362);

        // . Draw Player Body & Limbs
        glUniform2f(translationLoc, playerX, 0.0f);
        glUniform3f(colorLoc, 0.0f, 0.0f, 1.0f);
        glBindVertexArray(playerLinesVAO);
        glDrawArrays(GL_LINES, 0, 8);

        // . Draw Basketball
        glUniform2f(translationLoc, ballX, ballY);
        glUniform3f(colorLoc, 1.0f, 0.5f, 0.0f);
        glBindVertexArray(ballVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 362);

        // . Draw Basket Pole, Board & Ring
        glUniform2f(translationLoc, 0.0f, 0.0f);
        glBindVertexArray(basketVAO);
        
        // Pole (White)
        glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f);
        glDrawArrays(GL_LINES, 0, 2);
        
        // Board (White/Inherited state from 2nd code)
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        // Ring (Red)
        glUniform3f(colorLoc, 1.0f, 0.0f, 0.0f);
        glDrawArrays(GL_LINES, 6, 2);

 
        char title[50];
        sprintf(title, "Basketball Game | Score: %d", score);
        glfwSetWindowTitle(window, title);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // memory cleanup
    glDeleteVertexArrays(1, &groundVAO); glDeleteBuffers(1, &groundVBO); glDeleteBuffers(1, &groundEBO);
    glDeleteVertexArrays(1, &basketVAO); glDeleteBuffers(1, &basketVBO); glDeleteBuffers(1, &boardEBO);
    glDeleteVertexArrays(1, &playerLinesVAO); glDeleteBuffers(1, &playerLinesVBO);
    glDeleteVertexArrays(1, &headVAO); glDeleteBuffers(1, &headVBO);
    glDeleteVertexArrays(1, &ballVAO); glDeleteBuffers(1, &ballVBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_A && playerX > -90)
            playerX -= 5;
        if (key == GLFW_KEY_D && playerX < 40)
            playerX += 5;
        if (key == GLFW_KEY_SPACE && !shooting) {
            shooting = true;
            velocityX = 1.5;
            velocityY = 4.5;
        }
    }
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// circle generate helper function
void setupCircleVAO(unsigned int &vao, unsigned int &vbo, float radius, int segments) {
    std::vector<float> vertices;
    vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(0.0f); // Center

    for (int i = 0; i <= segments; i++) {
        float angle = i * 3.1415926f / 180.0f;
        vertices.push_back(cos(angle) * radius);
        vertices.push_back(sin(angle) * radius);
        vertices.push_back(0.0f);
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

// Shader compile checker
void checkShaderCompile(unsigned int shader, std::string type) {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << std::endl;
        }
    }
}