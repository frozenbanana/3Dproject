//HB-branch latest push
#include <Windows.h>

#include <stdio.h>
#include <string>
#include <fstream>
#include <streambuf>

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <JM\camJM.h>
#include <JM\shaderJM.h>
#include <JM\meshJM.h>
#include <JM\modelJM.h>
#include <HB\shaderHB.h>

// Paths 
const GLchar* vsPath = "shaders/JM/VertexShader.glsl";
const GLchar* gsPath = "shaders/JM/GeometryShader.glsl";
const GLchar* fsPath = "shaders/JM/FragmentShader.glsl";

// namespace
using namespace std;

//Constants
const unsigned int WINDOW_WIDTH = 960;  //640;
const unsigned int WINDOW_HEIGHT = 720; //480;

										//NTS: For testing, check which should be handled differently
GLuint vertexAtt = 0;
GLuint vertexBuf = 0;
GLuint objectTex = 0;

//Array in which the data for key presses is stored (used in key_callback())
bool key_data[1024];

//Variables for tracking mouse (used in mouse_callback())
bool firstMouse = true;
GLfloat lastX = WINDOW_WIDTH / 2.0;
GLfloat lastY = WINDOW_HEIGHT / 2.0;
GLfloat yaw = -90.0f;
GLfloat pitch = 0.0f;


//Define the implication of the buffer offset
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

//Functions
GLFWwindow* createWindow();
void createTestVertices(JMshader* shaderPro);
void createTestMesh();
void key_callback(GLFWwindow* winPtr, int key, int scan, int act, int mode);
void mouse_callback(GLFWwindow* winPtr, double xPos, double yPos);

int main() {
	//Variables
	GLFWwindow* windowPtr = NULL;

	glewExperimental = GL_TRUE;
	glewInit();

	if (!glfwInit()) {
		fprintf(stderr, "ERROR: GLFW3 could not start\n");
		return 1;
	}

	//create the matrices
	glm::mat4 world;				//NTS: All matrices start as identity matrices
	glm::mat4 view;
	glm::mat4 perspective;

	//Variables for tracking time
	GLfloat deltaTime = 0.0f;
	GLfloat lastFrame = 0.0f;

	//create camera
	JMcamera jmcamera(key_data, &yaw, &pitch, &deltaTime);

	//create variables for perspective
	double fov = 3.1415926535 * 0.45;
	double aspect = (double)(WINDOW_WIDTH / WINDOW_HEIGHT);
	double nearPlane = 0.1;
	double farPlane = 20.0;


	//------------------------------------------------------------------------------

	//Start a GL-context with the help of GLFW
	//If start-up fails write a message to stderr and end the program


	//Create a window. If the window isn't created properly the program doesn't run
	windowPtr = createWindow();
	if (windowPtr != NULL) {

		//Set glfw's context
		glfwMakeContextCurrent(windowPtr);

		//Set the window to capture the mouse and hide it. It's like a cat but digital
		glfwSetInputMode(windowPtr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		//Set callback functions
		glfwSetKeyCallback(windowPtr, key_callback);				//This sets the basic key_callback function 'key_callback' as default.
		glfwSetCursorPosCallback(windowPtr, mouse_callback);		//This sets the basic mouse_callback function 'mouse_callback' as default.

																	//initiate GLEW

				//create shaders
		JMshader baseShader(vsPath, gsPath, fsPath);				//Creates an object of the JMshader class and send in the .glsl-files for the shaders
		//Shader baseShader(vsPath, gsPath, fsPath);

																										//create vertices
																													//createTestVertices(&baseShader);	//Updated to recieve a pointer to an object from the JMshader class

																													//create model
																													//JMmodel testModel("OBJ_Files/cube_green_phong_12_tris_QUADS.obj");
		//JMmodel testModel("resoures/nanosuit/nanosuit.obj");

		//enable depth testing and use the lesser value as "closer to the camera"
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		//base alterations to the matrices
		//view = jmcamera.getView();
		perspective = glm::perspective(fov, aspect, nearPlane, farPlane);


		//tell the shader program where to get the uniform matrices		//shaderPro -> baseShader.program
		GLint uniWorld = glGetUniformLocation(baseShader.program, "world");
		GLint uniView = glGetUniformLocation(baseShader.program, "view");
		GLint uniPerspective = glGetUniformLocation(baseShader.program, "perspective");

		//createTestVertices(&baseShader);
		//create a window loop
		while (!glfwWindowShouldClose(windowPtr)) {

			//update matrices
			//"world-update"
			view = jmcamera.getView();		//"view-update"
											//view = hbcamera.getView();	// same function, uncomment the one needed
											//"perspective-update"

											//upload the matrices
			glUniformMatrix4fv(uniWorld, 1, GL_FALSE, glm::value_ptr(world));
			glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(uniPerspective, 1, GL_FALSE, glm::value_ptr(perspective));

			//render
			glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			baseShader.useProgram();
			glBindVertexArray(vertexAtt);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
			//testModel.drawModel(&baseShader);
			GLfloat curFrame = (GLfloat)glfwGetTime();	//jm: I have been thinking if we should have this* in the beginning of every loop
														//but I do not think it does any difference, right? The rest of the code will take
														//the same time anyway. *(by this I mean the time, pollEvents and movement code)
			deltaTime = curFrame - lastFrame;
			lastFrame = curFrame;


			glfwPollEvents();								//NTS:	This function checks for new events (key presses or mouse movements etc)
															//		It then calls the corresponding function. Which function this is can be
															//		set using callback methods


															//hbcamera.do_movement();			// hb: Uses the data to make a camera manipulation hence movement
			jmcamera.camMove();


			glfwSwapBuffers(windowPtr);
		}

	}

	glfwTerminate();
	return 0;
}


GLFWwindow* createWindow() {
	//Use the GLFW to create a window. Check if it has started as intended. If it hasn't, return NULL and write a message to stderr
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "The Project", NULL, NULL);
	if (!window) {
		fprintf(stderr, "ERROR: GLFW3 could not open the window");
		window = NULL;
	}
	return window;
}


void createTestVertices(JMshader* shaderPro) {

	//Creating a triangle to test with
	struct TVertex {
		float x, y, z;
		float r, g, b;
	};

	TVertex triangle[3] = {
		{ 0.0f, -0.5f, 0.0f,	1.0f, 0.0f, 0.0f },
		{ -0.5f, 0.5f, 0.0f,	0.0f, 1.0f, 0.0f },
		{ 0.5f, 0.5, 0.0f,		0.0f, 0.0f, 1.0f }
	};

	//Enable VAO
	glGenVertexArrays(1, &vertexAtt);
	glBindVertexArray(vertexAtt);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	//Create VBO id
	glGenBuffers(1, &vertexBuf);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);

	//Tell the program where to find the placement of a vertex
	GLuint vertexPos = glGetAttribLocation((*shaderPro).program, "vertex_position");							//shaderPro -> (*shaderPro).program
	glVertexAttribPointer(vertexPos, 3, GL_FLOAT, GL_FALSE, sizeof(TVertex), BUFFER_OFFSET(0));

	GLuint vertexCol = glGetAttribLocation((*shaderPro).program, "vertex_color");
	glVertexAttribPointer(vertexCol, 3, GL_FLOAT, GL_FALSE, sizeof(TVertex), BUFFER_OFFSET(sizeof(float) * 3));
}

void createTestMesh() {
	//TBA
}

void key_callback(GLFWwindow* winPtr, int key, int scan, int act, int mode) {
	/*
	'win' is a pointer to the window active
	'key' tells of which key was pressed
	'scan' ...?
	'act' tells if the key is pressed or released
	'mode' tells stuff like 'shift' or 'ctrl' has been pressed
	*/

	if (key == GLFW_KEY_ESCAPE && act == GLFW_PRESS) {
		glfwSetWindowShouldClose(winPtr, GL_TRUE);
	}


	// ! setting global variable
	//Store the data from when a key is pressed
	if (key >= 0 && key < 1024)					// hb: Added this safety feature
	{
		if (act == GLFW_PRESS) {
			key_data[key] = true;
		}
		else if (act == GLFW_RELEASE) {
			key_data[key] = false;
		}
	}
}

void mouse_callback(GLFWwindow* winPtr, double xPos, double yPos) {

	if (firstMouse) {			//Prevents the camera from jumping upon first moving into the window
		lastX = (GLfloat)xPos;
		lastY = (GLfloat)yPos;
		firstMouse = false;
	}

	GLfloat xOffset = (GLfloat)xPos - lastX;
	GLfloat yOffset = lastY - (GLfloat)yPos;		//y goes from top to bottom
	lastX = (GLfloat)xPos;
	lastY = (GLfloat)yPos;

	GLfloat sensitivity = 0.05f;
	xOffset = xOffset * sensitivity;
	yOffset = yOffset * sensitivity;

	yaw = yaw + xOffset;
	pitch = pitch + yOffset;

	if (pitch > 89.0f) {
		pitch = 89.0f;
	}
	if (pitch < -89.0f) {
		pitch = -89.0f;
	}

}


