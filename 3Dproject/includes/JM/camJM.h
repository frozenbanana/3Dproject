#ifndef CAMJM_H
#define CAMCJM_H

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

//The camera class
class JMcamera {
private:
	//Camera values
	glm::vec3 camPos;
	glm::vec3 camFront;
	glm::vec3 camUp;

	glm::mat4 camView;		//A matrix used to manipulate view-space

	GLfloat camSpeed;

	bool* keyPtr;			//A pointer to the array storing key values
	GLfloat* yawPtr;
	GLfloat* pitchPtr;
	GLfloat* deltaTimePtr;

	GLfloat curYaw;			//Variables to save the current angles in yaw and pitch
	GLfloat curPitch;

	void updateView();
	void wasdUpdate();
	void mouseUpdate();
	
public:
	JMcamera(bool* key_dataArr, GLfloat* yaw_Ptr, GLfloat* pitch_Ptr, GLfloat* deltaTime_Ptr);
	~JMcamera();

	glm::mat4 getView();
	glm::vec3 getPosition();
	void camMove();
};


#endif
