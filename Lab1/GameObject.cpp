#include "GameObject.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

GameObject::GameObject()
{
	active = true;
	this->forward = glm::vec3(0.0f, -1.0f, 0.0f);
	this->up = glm::vec3(0.0f, 0.0f, 1.0f);

}

void GameObject::draw(Mesh *mesh)
{
	mesh->draw();
}

void GameObject::setActive(bool set)
{
	active = set;
}

void GameObject::update(Mesh* mesh)
{
	mesh->updateSphereData(*tObject.GetPos(), 0.62f);
}

void GameObject::moveForward(float amt)
{
	pos = glm::vec3(tObject.GetPos()->x, tObject.GetPos()->y, tObject.GetPos()->z);
	pos += forward * amt;
	tObject.SetPos(pos);
}

void GameObject::moveBack(float amt)
{
	pos = glm::vec3(tObject.GetPos()->x, tObject.GetPos()->y, tObject.GetPos()->z);
	pos -= forward * amt;
	tObject.SetPos(pos);
}

void GameObject::moveDown(float amt)
{
	pos = glm::vec3(tObject.GetPos()->x, tObject.GetPos()->y, tObject.GetPos()->z);
	pos -= up * amt;
	tObject.SetPos(pos);
}

void GameObject::moveUp(float amt)
{
	pos = glm::vec3(tObject.GetPos()->x, tObject.GetPos()->y, tObject.GetPos()->z);
	pos += up * amt;
	tObject.SetPos(pos);
}

void GameObject::turnObject(float angle)
{
	// Work out rotation angle
	glm::mat4 rotation = glm::rotate(angle, up);

	forward = glm::vec3(glm::normalize(rotation * glm::vec4(forward, 0.0f)));

}

void GameObject::setForward(glm::vec3 forwardSet)
{
	forward = forwardSet;
}

glm::vec3 GameObject::getForward()
{
	return forward;
}

float GameObject::getAngle()
{
	return turnAngle;
}

void GameObject::orbit(glm::vec3 modelPos)
{
	// Find new Up angle relative to orbited object
	newUp = -glm::vec3(glm::normalize(modelPos - glm::vec3(tObject.GetPos()->x, tObject.GetPos()->y, tObject.GetPos()->z)));

	// Find the angle between the new up and the old up
	float dot = up.x * newUp.x + up.y * newUp.y + up.z * newUp.z;
	float upSq = up.x * up.x + up.y * up.y + up.z * up.z;
	float newUpSq = newUp.x * newUp.x + newUp.y * newUp.y + newUp.z * newUp.z;

	float angle = acos(dot / sqrt(upSq * newUpSq));

	// Calculate right vector
	glm::vec3 right = glm::normalize(glm::cross(up, forward));
	// Rotate forward vector relative to the angle of pitch change
	forward = glm::vec3(glm::normalize(glm::rotate(angle, right) * glm::vec4(forward, 0.0)));
	up = newUp;
}

void GameObject::transformPositions(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale)
{
	tObject.SetPos(pos);
	tObject.SetRot(rot);
	tObject.SetScale(scale);
}