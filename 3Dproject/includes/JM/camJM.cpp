// Functions made by Jakob Månsson, 960520-7373
#include "camJM.h"

//Camera class
//Private
void JMcamera::updateView() {
	this->camView = glm::lookAt(this->camPos, this->camFront + this->camPos, this->camUp);
}

void JMcamera::wasdUpdate() {

	if (this->keyPtr[GLFW_KEY_W]) {
		this->camPos = this->camPos + this->camFront*this->camSpeed;
	}
	if (this->keyPtr[GLFW_KEY_S]) {
		this->camPos = this->camPos - this->camFront*this->camSpeed;
	}
	if (this->keyPtr[GLFW_KEY_A]) {
		this->camPos = this->camPos - glm::normalize(glm::cross(this->camFront, this->camUp))*this->camSpeed;
	}
	if (this->keyPtr[GLFW_KEY_D]) {
		this->camPos = this->camPos + glm::normalize(glm::cross(this->camFront, this->camUp))*this->camSpeed;
	}
}

void JMcamera::mouseUpdate() {

	glm::vec3 stare;
	
	stare.x = cos(glm::radians(*(this->yawPtr))) * cos(glm::radians(*(this->pitchPtr)));
	stare.y = sin(glm::radians(*(this->pitchPtr)));
	stare.z = sin(glm::radians(*(this->yawPtr))) * cos(glm::radians(*(this->pitchPtr)));

	this->camFront = glm::normalize(stare);

}

//Public
JMcamera::JMcamera(bool* key_dataArr, GLfloat* yaw_Ptr, GLfloat* pitch_Ptr, GLfloat* deltaTime_Ptr) {
	camPos = glm::vec3(0.0f, 0.0f, 2.0f);		//NTS: Had a negative value
	camFront = glm::vec3(0.0f, 0.0f, -1.0f);
	camUp = glm::vec3(0.0f, 1.0f, 0.0f);

	camView = glm::lookAt(camPos, camFront + camPos, camUp);
	//Move to camPos (0, 0, 2)
	//Face look towards camFront (0, 0, -1) + an alteration depending on the position of camera
	//Set upwards to be camUp (0, 1, 0)

	camSpeed = 0.1f;

	keyPtr = key_dataArr;
	yawPtr = yaw_Ptr;
	pitchPtr = pitch_Ptr;
	deltaTimePtr = deltaTime_Ptr;

	curYaw = *(yaw_Ptr);
	curPitch = *(pitch_Ptr);
}

JMcamera::~JMcamera() {

};

glm::mat4 JMcamera::getView() {
	return this->camView;
}

void JMcamera::camMove(){

	this->camSpeed = (GLfloat)(1.0 * *(this->deltaTimePtr));

	this->mouseUpdate();

	this->wasdUpdate();

	this->updateView();
}

