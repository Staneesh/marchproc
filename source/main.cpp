#include "glad.h"
#include "glad.h"
#include <GLFW/glfw3.h>
#include <cstring>


#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void logError(const char* s)
{
    printf("[ERROR]: %s\n", s);
}

void loadEntireFile(const char* path, unsigned char** source)
{
    FILE* file = fopen(path, "r");
	if (file == nullptr)
	{
		logError("Unable to open file!");
	}
    
    fseek(file, 0, SEEK_END);
    unsigned fileSize = ftell(file); 
    fseek(file, 0, SEEK_SET);

    *source = (unsigned char *)calloc(1, fileSize + 1);
    memset(*source, 0, fileSize + 1);

    if (fread(*source, fileSize, 1, file) != 1) {
        logError("Failed reading file!");
        logError(path);
        
        return;
    }
         
    fclose(file);
}

unsigned createShaderProgram(const char* pathToVS, const char* pathToFS)
{
    unsigned vertex, fragment;
    unsigned program;
    int success;
    char infoLog[512];

    unsigned char* vsCode;
    unsigned char* fsCode;
    loadEntireFile(pathToVS, &vsCode);
    loadEntireFile(pathToFS, &fsCode);
    
    vertex = glCreateShader(GL_VERTEX_SHADER);
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vertex, 1, (const char**)&vsCode, NULL);
    glShaderSource(fragment, 1, (const char**)&fsCode, NULL);
    
    glCompileShader(vertex);
    glCompileShader(fragment);
    
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        printf("vertex shader error: %s\n", infoLog);
    }

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        printf("fragment shader error: %s\n", infoLog);
    }
    
    program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        printf("%s\n", infoLog);     
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    free(vsCode);
    free(fsCode);
    
    return program;
}




int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "marchproc", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    unsigned ourShader = createShaderProgram
		("source/shader.vs", "source/shader.fs"); 

    float vertices[] = {
        1.0f,  1.0f, 0.0f,  // top right
        1.0f, -1.0f, 0.0f,  // bottom right
       -1.0f, -1.0f, 0.0f,  // bottom left
       -1.0f,  1.0f, 0.0f   // top left 
   };
   unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

   unsigned int VBO, VAO, EBO;
   glGenVertexArrays(1, &VAO);
   glGenBuffers(1, &VBO);
   glGenBuffers(1, &EBO);
   // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
   glBindVertexArray(VAO);

   glBindBuffer(GL_ARRAY_BUFFER, VBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);

   // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
   glBindBuffer(GL_ARRAY_BUFFER, 0); 

   // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
   //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

   // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
   // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
   glBindVertexArray(0); 
    


    while (!glfwWindowShouldClose(window))
    {
    // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(ourShader);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); // no need to unbind it every time 
 
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        
    }

    return 0;
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
