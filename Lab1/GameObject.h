#pragma once

#include "Mesh.h"
#include "transform.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>


class GameObject
{

public:
	GameObject();
	void transformPositions(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale);
	void update(Mesh* mesh);
	void draw(Mesh* mesh);
	void setActive(bool set);
	void moveForward(float amt);
	void moveBack(float amt);
	void moveDown(float amt);
	void moveUp(float amt);
	void turnObject(float angle);
	void orbit(glm::vec3 modelPos);
	void setForward(glm::vec3 forwardSet);
	glm::vec3 getForward();
	float getAngle();

	bool getActive() { return active; }
	glm::mat4 getModel() { return tObject.GetModel(); }
	Transform getTM() { return tObject; }


private:
	Transform tObject;
	Camera cameraIn;
	Shader shaderIn;
	glm::vec3 forward;
	glm::vec3 up;
	glm::vec3 newUp;
	glm::vec3 pos;
	
	bool active;
	float counter = 0.0f;
	float turnAngle = 0.0f;

	float deltaTime;
};


