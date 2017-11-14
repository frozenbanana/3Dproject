#pragma once
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>

class Camera
{
public:
	Camera(const glm::vec3& pos, float fov, float aspect, float zNear, float zFar)
	{
		this->m_perspective = glm::perspective(fov, aspect, zNear, zFar);
		this->m_position = pos;
		this->m_forward = glm::vec3(0, 0, 1.f);
		this->m_up = glm::vec3(0, 1.f, 0);
	}

	inline glm::mat4 GetViewProjection() const
	{
		return this->m_perspective * glm::lookAt(this->m_position, this->m_position + this->m_forward, this->m_up);
	}

	~Camera(){}

private:
	glm::mat4 m_perspective;
	glm::vec3 m_position;
	glm::vec3 m_forward;
	glm::vec3 m_up;
};

