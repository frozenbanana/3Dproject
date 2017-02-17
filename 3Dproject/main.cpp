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

// Includes by Jakob Månsson
#include <JM\camJM.h>
#include <JM\shaderJM.h>
#include <JM\meshJM.h>
#include <JM\modelJM.h>

// Includes by Henry Bergström
#include <HB\shaderHB.h>
#include <HB\modelHB.h>
#include <HB\meshHB.h>

// Paths 
const GLchar* def_geo_vsPath = "shaders/HB/deferred_geoPass_vs.glsl";
const GLchar* def_geo_fsPath = "shaders/HB/deferred_geoPass_frag.glsl";
const GLchar* def_light_vsPath = "shaders/HB/deferred_lightPass_vs.glsl";
const GLchar* def_light_fsPath = "shaders/HB/deferred_lightPass_frag.glsl";;
const GLchar* gsPath = "shaders/HB/GeometryShader.glsl";
const GLchar* fsPath = "shaders/HB/FragmentShader.glsl";

// Namespace
using namespace std;

// Constants
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
void createGBuffer();

int main() {
	//Variables
	GLFWwindow* windowPtr = NULL;


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
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: GLFW3 could not start\n");
		return 1;
	}

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
		glewExperimental = GL_TRUE;
		GLenum err = glewInit();
		if (err != GLEW_OK)
		{
			cout << "ERROR: GLEW INIT IS NOT OK" << endl;
		}


		//create shaders
		//JMshader baseShader(vsPath, gsPath, fsPath);				//Creates an object of the JMshader class and send in the .glsl-files for the shaders
		Shader geoShader(def_geo_vsPath, def_geo_fsPath);
		Shader lightShader(def_light_vsPath, def_light_fsPath);

		//create vertices
		//createTestVertices(&baseShader);	//Updated to recieve a pointer to an object from the JMshader class

		//create model
		//JMmodel testModel("OBJ_Files/cube_green_phong_12_tris_QUADS.obj");
		//JMmodel testModel("resources/nanosuit/nanosuit.obj");
		ModelHB testModel("resources/nanosuit/nanosuit.obj");

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
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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

void createGBuffer()
{
	// CREATE G-Buffer
	// One texture for every attribute
	// 3 textures:
	// 1 - Position
	// 2 - Color
	// 3 - Normals
	// Set up G-Buffer
	// 3 textures:
	// 1. Positions (RGB)
	// 2. Color (RGB) + Specular (A)
	// 3. Normals (RGB) 
	GLuint gBuffer;
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	GLuint gPosition, gNormal, gAlbedoSpec;
	// - Position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
	// - Normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
	// - Color + Specular color buffer
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
	// - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);
	// - Create and attach depth buffer (renderbuffer)
	GLuint rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WINDOW_WIDTH, WINDOW_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// - Finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}
