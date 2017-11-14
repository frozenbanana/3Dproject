#pragma once
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>

class Transform
{
public:
	Transform(const glm::vec3& pos = glm::vec3(), const glm::vec3& rot = glm::vec3(), const glm::vec3& scale = glm::vec3(1.f, 1.f, 1.f))
	{
		this->m_pos = pos;
		this->m_rot = rot;
		this->m_scale = scale;
	}

	inline glm::mat4 GetModelMatrix() const
	{
		glm::mat4 posMatrix = glm::translate(this->m_pos);
		glm::mat4 rotXMatrix = glm::rotate(this->m_rot.x, glm::vec3(10.0f,0.0,0.0));
		glm::mat4 rotYMatrix = glm::rotate(this->m_rot.y, glm::vec3(0.0f,1.0,0.0));
		glm::mat4 rotZMatrix = glm::rotate(this->m_rot.z, glm::vec3(0.0f,0.0,1.0));
		glm::mat4 scaleMatrix = glm::scale(this->m_scale);

		glm::mat4 rotMatrix = rotZMatrix * rotYMatrix * rotXMatrix;

		return posMatrix * rotMatrix * scaleMatrix;
	}

	inline glm::vec3& GetPos() { return this->m_pos; };
	inline glm::vec3& GetRot() { return this->m_rot; };
	inline glm::vec3& GetScale() { return this->m_scale; };

	inline void SetPos(const glm::vec3& pos) {this->m_pos = pos;};
	inline void SetRot(const glm::vec3& rot) {this->m_pos = rot;};
	inline void SetScale(const glm::vec3& scale) {this->m_pos = scale;};

	~Transform(){}
private:
	glm::vec3 m_pos;
	glm::vec3 m_rot;
	glm::vec3 m_scale;
};

