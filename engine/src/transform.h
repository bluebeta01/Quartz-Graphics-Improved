#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/quaternion.hpp>
#include <gtc/quaternion.hpp>

namespace Quartz
{
struct Transform
{
	Transform(glm::vec3 position = glm::vec3(0,0,0), glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
		glm::vec3 scale = glm::vec3(1,1,1)) :
		position(position), rotation(rotation), scale(scale)
	{
	}

	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

	void rotateX(float angle)
	{
		glm::quat rot = glm::angleAxis(angle, glm::vec3(1, 0, 0));
		rotation = rot * rotation;
	}
	void rotateY(float angle)
	{
		glm::quat rot = glm::angleAxis(angle, glm::vec3(0, 1, 0));
		rotation = rot * rotation;
	}

	static glm::mat4 matrixFromTransform(const Transform& transform, bool correctRotation)
	{
		glm::mat4 positionMatrix = glm::translate(glm::mat4(1.0f), transform.position);
		glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), transform.scale);
		return positionMatrix * glm::toMat4(transform.rotation) * scaleMatrix;
	}
};
}