/******************************************************************************
	CSE 389-02
	File Name: model_view.cpp
	Synopsis: This file includes an object class that will load a simple model
              from an object file, as well as a main driver that will render
              the loaded models onto the viewing window.  You will also be
              able to dynamically switch between your simple loaded objects.
******************************************************************************/

#include "Object.h"
#include "classes/camera.h"

/******************************************************************************

	Shader Set-up

******************************************************************************/

const char *vertexShaderSource = "#version 150\n"
        "in vec4 vPosition;\n"
        "in vec4 vColor;\n"
        "out vec4 color;\n"
        "uniform vec3 theta;\n"
        "uniform mat4 MVP;"
        "void main()\n"
        "{\n"
        "   color = vColor;\n"
        "   gl_Position = MVP * vPosition;\n"
        "}\0";

const char *fragmentShaderSource = "#version 150\n"
        "in vec4 color;\n"
        "out vec4 fColor;\n"
        "void main()\n"
        "{\n"
        "   fColor = color;\n"
        "}\n\0";

/******************************************************************************

	GLOBAL VARIABLES

******************************************************************************/

// Array of rotation angles (degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int Axis = Xaxis;

const int NUM_MODELS = 2;

// You will want to change these .obj files to the ones you are using
Object objects[NUM_MODELS] = {std::string("/home/charles/CLionProjects/Glitter/Sources/cube.obj"),
                              std::string("/home/charles/CLionProjects/Glitter/Sources/museum.obj")};
GLuint vao[NUM_MODELS];
int modelSelection = 0;
bool Lpressed = false;
bool Upressed = false;
bool Dpressed = false;
bool Rpressed = false;
bool Ipressed = false;
bool Opressed = false;

Camera camera;

// MVP matrix
GLint MVP_Matrix;

// set up our window size
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

/******************************************************************************

	OPENGL INITIALIZATION

******************************************************************************/
void init() {
    // create a vertex array object (vao)
    glGenVertexArrays(NUM_MODELS, vao);

    // Build and compile our shader program
    // ---------- Vertex Shader -----------
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    // ---------- Fragment Shader ----------
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    // Link the shaders to the program
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    // check for linking errors
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED.\n"
                  << infoLog << std::endl;
    }

    // now lets get rid of the shaders since
    // we have linked them to the program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // now let's link all this stuff up to our VAO
    // and prep to display our objects
    glUseProgram(program);
    for(int i = 0; i < NUM_MODELS; i++){
        glBindVertexArray(vao[i]);
        objects[i].load(program);
    }

    glBindVertexArray(vao[0]);
    MVP_Matrix = glGetUniformLocation(program, "MVP");

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0,0.0, 1.0);

} // end init()

/******************************************************************************

	DISPLAY LOADED OBJECT

*******************************************************************************/
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glUniform3fv(theta, 1, Theta);
    glUniformMatrix4fv(MVP_Matrix, 1, GL_FALSE, &camera.GetViewMatrix()[0][0]);

    // draw the currently loaded model
    objects[modelSelection].draw();
} // end display()

/******************************************************************************

	KEYBOARD SWITCH COMMANDS

******************************************************************************/
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        exit(EXIT_SUCCESS);
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        modelSelection = (modelSelection + 1) % NUM_MODELS;
        glBindVertexArray(vao[modelSelection]);
    }
    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        Lpressed = true;
    }
    if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
        Lpressed = false;
    }
    if (key == GLFW_KEY_D && action == GLFW_PRESS) {
        Rpressed = true;
    }
    if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
        Rpressed = false;
    }
    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        Upressed = true;
    }
    if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
        Upressed = false;
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        Dpressed = true;
    }
    if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
        Dpressed = false;
    }
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
        Ipressed = true;
    }
    if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE) {
        Ipressed = false;
    }
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
        Opressed = true;
    }
    if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE) {
        Opressed = false;
    }
} // end key_callback()

/******************************************************************************

	MOUSE COMMANDS

******************************************************************************/
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    std::cout << "button: " << button << "mod: " << mods << std::endl;
} // end mouse_button_callback()

/******************************************************************************

	CAMERA UPDATES

******************************************************************************/
void idle() {
    if(Lpressed) {
        camera.ProcessKeyboard(LEFT, 0.005f);
    }
    if(Rpressed) {
        camera.ProcessKeyboard(RIGHT, 0.005f);
    }
    if(Upressed) {
        camera.ProcessKeyboard(FORWARD, 0.005f);
    }
    if(Dpressed) {
        camera.ProcessKeyboard(BACKWARD, 0.005f);
    }
    if(Ipressed) {
        camera.ProcessMouseMovement(5.0f, 0.000f);
    }
    if(Opressed) {
        camera.ProcessMouseMovement(-5.0f, 0.000f);
    }
} // end idle()

/******************************************************************************

	MAIN FUNCTION: GLFW WINDOW CREATION AND BASIC RENDER LOOP

******************************************************************************/
int main(){
    // initialize the window
    glfwInit();

    // require OpenGL context to support OpenGL 4.5 at minimum
    // Note: If your system cannot support 4.5, that is perfectly
    //       fine, just change the minimum requirements for major
    //       and minor context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

    // GLFW_OPENGL_PROFILE specifies that we want a context that
    // only supports the core functionality
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Let's create a windowed window ... something that is not
    // full screen
    GLFWwindow* window =
            glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT,
                             "CSE 389-02 Object Viewer",
                             nullptr, nullptr
            );

    // Check to see if your window started up correctly
    if(window == nullptr){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // make the OpenGL context active
    glfwMakeContextCurrent(window);

    // glad: load all OpenGL function pointers
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // get the party started
    init();

    // set up the requirements for our key/mouse clicks
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // glfw uses a "closed event" loop, which means you only have to handle
    // events when you need to
    while (!glfwWindowShouldClose(window)) {
        //retrieve window events
        glfwSwapBuffers(window);
        display();
        idle();
        glfwPollEvents();
    }

    // woo ... we are finished, let's clean up after ourselves
    glfwTerminate();
    return 0;
}




