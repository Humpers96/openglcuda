#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>

void UserInput(GLFWwindow* window);
const char* LoadShader(const char* shaderPath);

const char* v_shader_code =  "#version 330 core\n"
                            "layout(location = 0) in vec3 vertexPosition;\n"
                            "void main()\n"
                            "{\n"
                            "gl_Position = vec4(vertexPosition.x, vertexPosition.y, vertexPosition.z, 1.0);\n"
                            "}\0";


const char* f_shader_code = "#version 330 core\n"
                            "out vec4 colour;\n"
                            "void main()\n"
                            "{\n"
                            "colour = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                            "}\0";

int main() {

    //---------------------------------------//
    //      GLFW, GLEW and OpenGL Setup      //
    //---------------------------------------//

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(1000, 750, "CUDA pending...", NULL, NULL);
    if (!window)
    {
        std::cout << "Window creation failed!\n";
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        std::cout << "Error initialising GLEW!\n";

    glViewport(0, 0, 1000, 750);

    std::cout << glGetString(GL_VERSION) << "\n";



    //--------------------//
    //    Shader Setup    //
    //--------------------//

    // Vertex shader object
    GLuint vertexShader;
    // Create shader object
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // "Attach" shader object to source code
    glShaderSource(vertexShader, 1, &v_shader_code, NULL);
    // Compile the shader
    glCompileShader(vertexShader);

    // Fragement shader object
    GLuint fragmentShader;
    // Create shader object
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // Attach shader source code to shader object
    glShaderSource(fragmentShader, 1, &f_shader_code, NULL);
    // Compile shader
    glCompileShader(fragmentShader);

    // Check for shader compilation errors
    int success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        std::cout << "Shader compilation error:\n";
        exit(-1);
    }

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        std::cout << "Shader compilation error:\n";
        exit(-1);
    }

    // Shader program object
    GLuint shader;
    // Create a program that returns an ID reference to object
    shader = glCreateProgram();
    // Attach the vertex shader to the shader program
    glAttachShader(shader, vertexShader);
    // Attach the fragment shader to the shader program
    glAttachShader(shader, fragmentShader);
    // Link the shaders to each other...I think
    glLinkProgram(shader);

    glGetProgramiv(shader, GL_LINK_STATUS, &success);

    if (!success)
    {
        std::cout << "Shader linking error!\n";
        exit(-1);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);



    //------------------------------//
    //      Manual Vertex Data      //
    //------------------------------//

    // Manual vertex array
    float vertices[]{
        -0.5f, -0.5f,  0.0f,
         0.5f, -0.5f,  0.0f,
         0.0f,  0.5f,  0.0f
    };

    // Vertex buffer object
    GLuint VBO;
    // Create buffer object
    glGenBuffers(1, &VBO);
    // Bind GL_ARRAY_BUFFER to VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Copy data to buffer (vertice -> VBO)
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // Instruct OpenGL how to interpret the data, per vertex, in the VBO via the vertex shader.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3* sizeof(float), (void*)0);
    /*
     -               0: Which vertex attribute to configure. "layout (location = 0)" in the v shader.
     -               3: Size of the attribute. vec3 so 3 values.
     -        GL_FLOAT: Type of data. vec3 vars are comprised of three floats.
     -        GL_FALSE: True/False as to whether data is noramlised.
     - 3*sizeof(float): Stride. The "distance" between one set of vertex data and the next.

     The VBO that the data is taken from is dictated by whichever VBO is currently bound to 
     GL_ARRAY_BUFFER when glVertexAttribArray() is called.
    */

    // Enable the vertex attrribute 0 (the attribute located at 0 in the vertex shader)
    glEnableVertexAttribArray(0);

    //-----------------------//
    //      Render Loop      //
    //-----------------------//

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        UserInput(window);

        /* Render here */
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Instruct OpenGL to use previously created shader program
        glUseProgram(shader);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}



//----------------------------//
//      Helper Functions      //
//----------------------------//

void UserInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        exit(0);
    }
}

//const char* LoadShader(const char* shaderPath) {
//
//    std::string sourceCode;
//    std::ifstream fileStream;
//
//    try {
//        fileStream.open(shaderPath);
//
//        std::stringstream sStream;
//        sStream << fileStream.rdbuf();
//        fileStream.close();
//        sourceCode = sStream.str();
//    }
//    catch (std::ifstream::failure exception) {
//        std::cout << "Error reading shader files.\n";
//    }
//
//    return sourceCode.c_str();
//}

void CheckShaders(GLuint vShader, GLuint fShader, GLuint shaderProgram) {

    int success;

    glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        std::cout << "Vertex shader compilation error!\n";
        exit(-1);
    }

    glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        std::cout << "Fragment shader compilation error!\n";
        exit(-1);
    }

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success)
    {
        std::cout << "Shader linking error!\n";
        exit(-1);
    }
}