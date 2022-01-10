#pragma once
#include <SDL\SDL.h>
#include <SDL/SDL_mixer.h>
#include <GL/glew.h>
#include "Display.h" 
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "transform.h"
#include "SkyBox.h"
#include "GameObject.h"
#include "SDLAudio.h"
#include "DeltaTime.h"

// States Game is in
enum class GameState{PLAY, EXIT};

// States AI is in
enum class AIState{TRACK, ATTACK};

class MainGame
{
public:
	MainGame();
	~MainGame();

	void run();

private:

	void initSystems();
	void processInput();
	void gameLoop();
	void drawGame();
	void linkFogShader();
	void linkToon();
	void linkRimLighting();
	void linkGeo();
	void linkEmapping();

	void drawPlanet();
	void drawSkyBox();
	void drawAsteriods();
	void drawPlayerShip();
	void drawAIShip();
	void drawMissiles();

	void fireMissiles(GameObject ship);
	void initModels(GameObject*& asteroid);
	bool collision(glm::vec3 m1Pos, float m1Rad, glm::vec3 m2Pos, float m2Rad);
	void runAI();

	void createScreenQuad();

	void generateFBO(float w, float h);
	void bindFBO();
	void unbindFBO();
	void renderFBO();

	//void playAudio(unsigned int Source, glm::vec3 pos);

	Display _gameDisplay;
	GameState _gameState;
	AIState _aiState;
	Mesh planetMesh;
	Mesh rockMesh;
	Mesh shipMesh;
	Mesh missileMesh;
	Camera myCamera;
	Shader fogShader;
	Shader toonShader;
	Shader rimShader;
	Shader geoShader;
	Shader shaderSkybox;
	Shader eMapping;
	Shader basicTexture;
	Shader FBOShader;
	SDLAudio gameAudio;
	DeltaTime frameTimer;

	Transform transform;
	GameObject* asteroid = new GameObject[20];
	GameObject* missiles = new GameObject[20];
	GameObject playerShip;
	GameObject aiShip;
	GameObject planet;
	Texture bTexture;
	Texture pTexture;

	GLuint FBO;
	GLuint RBO;
	GLuint CBO;

	GLuint quadVAO;
	GLuint quadVBO;


	glm::vec3 currentCamPos;
	glm::vec3 currentShipPos;

	Skybox skybox;

	vector<std::string> faces;
	
	//Audio audioDevice;
	bool look = true;
	bool shake = false;
	float counter;
	bool useDelta = true;
	float deltaTime = 0;
	int lastMissileFired = 0;
	float shakeTime = 1.5f;
	float currentShake;

	Uint64 NOW = SDL_GetPerformanceCounter();
	Uint64 LAST = 0;
};

