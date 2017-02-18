//HB-branch latest push
#include <Windows.h>

#include <stdio.h>
#include <string>
#include <fstream>
#include <streambuf>
#include <vector>

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
#include <HB\cameraHB.h>

// Paths 
const GLchar* def_geo_vsPath = "shaders/HB/deferred_geoPass_vs.glsl";
const GLchar* def_geo_fsPath = "shaders/HB/deferred_geoPass_frag.glsl";
const GLchar* def_light_vsPath = "shaders/HB/deferred_lightPass_vs.glsl";
const GLchar* def_light_fsPath = "shaders/HB/deferred_lightPass_frag.glsl";;
const GLchar* vsPath = "shaders/JM/VertexShader.glsl";
const GLchar* gsPath = "shaders/JM/GeometryShader.glsl";
const GLchar* fsPath = "shaders/JM/FragmentShader.glsl";

// Objects
#define NR_OF_OBJECTS 10
#define NR_OF_LIGHTS 32

// Namespace
using namespace std;

// Constants
const unsigned int WINDOW_WIDTH = 1600;		
const unsigned int WINDOW_HEIGHT = 800;	

										//NTS: For testing, check which should be handled differently
GLuint vertexAtt = 0;
GLuint vertexBuf = 0;
GLuint objectTex = 0;
// G-BUFFER
GLuint gBuffer;
GLuint gPosition, gNormal, gDiffSpec;
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

//Functions JM
GLFWwindow* createWindow();
void createTestVertices(JMshader* shaderPro);
void createTestMesh();
void key_callback(GLFWwindow* winPtr, int key, int scan, int act, int mode);
void mouse_callback(GLFWwindow* winPtr, double xPos, double yPos);

// Functions HB
void createGBuffer();
vector<glm::vec3> createObjectPositions(GLuint nrOfObj);
vector<glm::vec3> createLightPositions(GLuint nrOfLi, float radiusLimit);
vector<glm::vec3> createLightColors(GLuint nrOfLi);
void renderQuad();
void drawTriangle(Shader shader);
void Do_Movement();

// camera global 
Camera hbcamera(glm::vec3(0.0f, 0.0f, 5.0f));


//Variables for tracking time
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

int main() {
	//Variables
	GLFWwindow* windowPtr = NULL;

	//create the matrices
	//glm::mat4 world;				//NTS: All matrices start as identity matrices
	//glm::mat4 view;
	//glm::mat4 perspective;


	//create camera
	//JMcamera jmcamera(key_data, &yaw, &pitch, &deltaTime);
	//Camera hbcamera(glm::vec3(0.0f, 0.0f, 5.0f));

	//create variables for perspective
	//double fov = 3.1415926535 * 0.45;
	//double aspect = (double)(WINDOW_WIDTH / WINDOW_HEIGHT);
	//double nearPlane = 0.1;
	//double farPlane = 20.0;

	//------------------------------------------------------------------------------//

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
		//Shader shader(vsPath,gsPath,fsPath);

		// Set lightShader to expect gPosition, gNormal and gDiffSpec
		lightShader.useProgram();
		glUniform1i(glGetUniformLocation(lightShader.program,"gPosition"), 0);
		glUniform1i(glGetUniformLocation(lightShader.program,"gNormal"), 1);
		glUniform1i(glGetUniformLocation(lightShader.program,"gDiffSpec"), 2);

		//create model
		//JMmodel testModel("OBJ_Files/cube_green_phong_12_tris_QUADS.obj");
		//JMmodel testModel("resources/nanosuit/nanosuit.obj");
		ModelHB testModel("resources/nanosuit/nanosuit.obj");
		//ModelHB testModel("C:/Users/henry_000/Documents/Visual\ Studio\ 2015/Projects/learnOpenGL/obj_files/nanosuit/nanosuit.obj");


		// Configure model position
		vector<glm::vec3> objectPositions = createObjectPositions(NR_OF_OBJECTS);
		std::vector<glm::vec3> lightPositions = createLightPositions(NR_OF_LIGHTS, 10.0f);
		std::vector<glm::vec3> lightColor = createLightColors(NR_OF_LIGHTS);

		//enable depth testing and use the lesser value as "closer to the camera"
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		// create gBuffer: 1 - Position, 2 - Normal, 3 - Diffuse + Specular
		createGBuffer();

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		//create a window loop
		while (!glfwWindowShouldClose(windowPtr)) {
			// Set frame time
			GLfloat currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			// Check and call events
			glfwPollEvents();
			Do_Movement();

			// 1. GEOMETRY PASS - DEFERRED RENDERING
			// Transformation matrices
			glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glm::mat4 projection = glm::perspective(hbcamera.getZoom(), (float)WINDOW_WIDTH/ (float)WINDOW_HEIGHT, 0.1f, 100.0f);
			glm::mat4 view = hbcamera.GetViewMatrix();
			geoShader.useProgram();
			glUniformMatrix4fv(glGetUniformLocation(geoShader.program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
			glUniformMatrix4fv(glGetUniformLocation(geoShader.program, "view"), 1, GL_FALSE, glm::value_ptr(view));

			// Draw the loaded model
			for (size_t i = 0; i < objectPositions.size(); i++)
			{
				glm::mat4 model;
				model = glm::translate(model, objectPositions[i]);								// Translate it down a bit so it's at the center of the scene
				model = glm::scale(model, glm::vec3(0.25f));					    // It's a bit too big for our scene, so scale it down
				glUniformMatrix4fv(glGetUniformLocation(geoShader.program, "model"), 1, GL_FALSE, glm::value_ptr(model));
				testModel.Draw(geoShader);
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// 2. LIGHTING PASS: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			lightShader.useProgram();
			// bind textures 0,1,2 to gPos.. gNor... gDif...
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, gPosition);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, gNormal);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, gDiffSpec);

			for (size_t i = 0; i < lightPositions.size(); i++)
			{
				glUniform3fv(glGetUniformLocation(lightShader.program, ("lights[" + std::to_string(i) + "].Position").c_str()), 1, &lightPositions[i][0]); // why like this format?
				glUniform3fv(glGetUniformLocation(lightShader.program, ("lights[" + std::to_string(i) + "].Color").c_str()), 1, &lightColor[i][0]);
				// Update attenuation parameters and calculate radius
				const GLfloat constant = 1.0; // Note that we don't send this to the shader, we assume it is always 1.0 (in our case)
				const GLfloat linear = 0.7;
				const GLfloat quadratic = 1.8;
				glUniform1f(glGetUniformLocation(lightShader.program, ("lights[" + std::to_string(i) + "].Linear").c_str()), linear);
				glUniform1f(glGetUniformLocation(lightShader.program, ("lights[" + std::to_string(i) + "].Quadratic").c_str()), quadratic);
			}

			glUniform3fv(glGetUniformLocation(lightShader.program, "viewPos"), 1, &hbcamera.position[0]);

			// Finally render quad
			renderQuad(); // How can this quad access the texture?

		    // Swap the buffers
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
	/*
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
	*/
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xoffset = xPos - lastX;
	GLfloat yoffset = lastY - yPos;

	lastX = xPos;
	lastY = yPos;

	hbcamera.ProcessMouseMovement(xoffset, yoffset);
}

void createGBuffer()
{
	// CREATE G-Buffer
	// One texture for every attribute
	// 3 textures:
	// 1 - Position
	// 3 - Normals
	// 2 - Color
	// Set up G-Buffer
	// 3 textures:
	// 1. Positions (RGB)
	// 3. Normals (RGB) 
	// 2. Color (RGB) + Specular (A)
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
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
	glGenTextures(1, &gDiffSpec);
	glBindTexture(GL_TEXTURE_2D, gDiffSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gDiffSpec, 0);
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
vector<glm::vec3> createObjectPositions(GLuint nrOfObj)
{
	std::vector<glm::vec3> objectPositions;
	// Check if neccessary 
	if (nrOfObj >= 1)
	{
		// create objects like an army
		float zPos = 0.0;
		float posOffset = 3.0;					// space between objects
		int inc = 0;

		for (size_t i = 0; i < nrOfObj; i++)
		{
			if (i % 5 == 0)						// max 5 on a row
			{
				inc = 0;
				zPos += posOffset;
			}
			objectPositions.push_back(glm::vec3(inc++ * posOffset, 0.0, zPos));
		}
	}

	return objectPositions;
}

vector<glm::vec3> createLightPositions(GLuint nrOfLi, float radiusLimit)
{
	std::vector<glm::vec3> lightPositions;
	if (nrOfLi >= 1)
	{	// randomize
		srand(13);
		float xPos, yPos, zPos;
		for (size_t i = 0; i < nrOfLi; i++)
		{	// random factor from 0.0 - 1.0
			xPos = ((rand() % 100) / radiusLimit);
			//yPos = ((rand() % 100) / radiusLimit);
			yPos = 0.0;
			zPos = ((rand() % 100) / radiusLimit);
			lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
			std::cout << "Light nr " << i << ": " << lightPositions[i].x <<", "<< lightPositions[i].y << ", " << lightPositions[i].z << std::endl;
		}

	}
	return lightPositions;
}

vector<glm::vec3> createLightColors(GLuint nrOfLi)
{
	std::vector<glm::vec3> lightColor;
	if (nrOfLi >= 1)
	{	// randomize
		srand(11);
		for (size_t i = 0; i < nrOfLi; i++)
		{	// random factor from 0.0 - 1.0
			// randomized color between 0.5-1.0
			GLfloat rColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
			GLfloat gColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
			GLfloat bColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
			lightColor.push_back(glm::vec3(rColor, gColor, bColor));
			
		}
	}
	return lightColor;
}

GLuint quadVAO = 0;
GLuint quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		GLfloat quadVertices[] = {
			// Positions        // Texture Coords
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		glGenVertexArrays(1, &quadVAO);
		glBindVertexArray(quadVAO);

		glGenBuffers(1, &quadVBO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void Do_Movement()
{
	// Camera controls
	if (key_data[GLFW_KEY_W])
		hbcamera.ProcessKeyboard(FORWARD, deltaTime);
	if (key_data[GLFW_KEY_S])
		hbcamera.ProcessKeyboard(BACKWARD, deltaTime);
	if (key_data[GLFW_KEY_A])
		hbcamera.ProcessKeyboard(LEFT, deltaTime);
	if (key_data[GLFW_KEY_D])
		hbcamera.ProcessKeyboard(RIGHT, deltaTime);
	if (key_data[GLFW_KEY_SPACE])
		hbcamera.ProcessKeyboard(UP, deltaTime);
}

// draw triangle
void drawTriangle(Shader shader)
{
	float points[] = {
		0.0f,  0.5f,  0.0f,
		0.5f, -0.5f,  0.0f,
		-0.5f, -0.5f,  0.0f
	};

	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), points, GL_STATIC_DRAW);

	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	// wipe the drawing surface clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader.useProgram();
	glBindVertexArray(vao);
	// draw points 0-3 from the currently bound VAO with current in-use shader
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);

}