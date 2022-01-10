#include "MainGame.h"
#include "Camera.h"
#include <iostream>
#include <string>

MainGame::MainGame()
{
	_gameState = GameState::PLAY;
	_aiState = AIState::TRACK;
	Display* _gameDisplay = new Display(); //new display
	DeltaTime* deltaTime = new DeltaTime();
}

MainGame::~MainGame()
{
}

void MainGame::run()
{
	initSystems(); 
	gameLoop();
}

void MainGame::initSystems()
{
	_gameDisplay.initDisplay(); 
	//whistle = audioDevice.loadSound("..\\res\\bang.wav");
	//backGroundMusic = audioDevice.loadSound("..\\res\\background.wav");

	// Load models and textures
	bTexture.load("..\\res\\bricks.jpg");
	pTexture.load("..\\res\\PlanetSurface.jpg");
	planetMesh.loadModel("..\\res\\Planet2.obj");
	rockMesh.loadModel("..\\res\\Rock1.obj");
	shipMesh.loadModel("..\\res\\R33.obj");
	missileMesh.loadModel("..\\res\\R33.obj");
	fogShader.init("..\\res\\fogShader.vert", "..\\res\\fogShader.frag"); //new shader
	toonShader.init("..\\res\\shaderToon.vert", "..\\res\\shaderToon.frag"); //new shader
	rimShader.init("..\\res\\shaderRim.vert", "..\\res\\shaderRim.frag");
	eMapping.init("..\\res\\shaderReflection.vert", "..\\res\\shaderReflection.frag");
	FBOShader.init("..\\res\\FBOShader.vert", "..\\res\\FBOShader.frag");
	basicTexture.init("..\\res\\Shader.vert", "..\\res\\Shader.frag");

	// Load background audio
	gameAudio.addAudioTrack("..\\res\\background.wav");

	initModels(asteroid);

	geoShader.initGeo();

	myCamera.initCamera(glm::vec3(0, 0, -50), 70.0f, (float)_gameDisplay.getWidth()/_gameDisplay.getHeight(), 0.01f, 1000.0f);

	generateFBO(_gameDisplay.getWidth(), _gameDisplay.getHeight());

	createScreenQuad();

	counter = 1.0f;

	vector<std::string> faces
	{
		"..\\res\\skybox\\right.png",
		"..\\res\\skybox\\left.png",
		"..\\res\\skybox\\top.png",
		"..\\res\\skybox\\bottom.png",
		"..\\res\\skybox\\front.png",
		"..\\res\\skybox\\back.png"
	};

	skybox.init(faces);
}

void MainGame::createScreenQuad()
{
	float quadVertices[] = { 
		// vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		 //positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f

		//// vertex attributes for a quad that fills the half of the screen
		/*-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f,  0.25f,  0.0f, 0.0f,
		-0.25f,  0.25f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		-0.25f,  0.25f,  1.0f, 0.0f,
		-0.25f,  1.0f,  1.0f, 1.0f*/
	};
	// cube VAO
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); //stride offset example
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

}


void MainGame::gameLoop()
{
	while (_gameState != GameState::EXIT)
	{
		// Set delta Time for current frame loop
		deltaTime = frameTimer.calculateDeltaTime(useDelta, 0.1f);
		
		gameAudio.playMusic();
		processInput();
		currentCamPos = myCamera.getPos();
		if (look)
		{
			myCamera.setLook(*playerShip.getTM().GetPos());
		}
		else
		{
			myCamera.setLook(*planet.getTM().GetPos());
		}
		

		// Check for collisions
		for (int i = 0; i < 20; i++)
		{
			// If ship has collided with an asteroid
			if (collision(*asteroid[i].getTM().GetPos(), rockMesh.getSphereRadius(), *playerShip.getTM().GetPos(), shipMesh.getSphereRadius()))
			{
				if (currentShake > 0)
				{
					//Shake screen and reduce health by one.
					shake = true;
					currentShake -= deltaTime;
				}
			}
			else
			{
			shake = false;
			currentShake = shakeTime;
			}

			// See if asteroid collided with any of the missiles
			for (int j = 0; j < 20; j++)
			{
				if (collision(*asteroid[i].getTM().GetPos(), rockMesh.getSphereRadius(), *missiles[j].getTM().GetPos(), missileMesh.getSphereRadius()))
				{
					// If asteroid hit missile then set astroid and missile active to false
					asteroid[i].setActive(false);
					missiles[j].setActive(false);
				}
			}
		}

		// Run AI
		runAI();

		// Draw frame of the game
		drawGame();
	}
}

void MainGame::processInput()
{
	SDL_Event evnt;

	while (SDL_PollEvent(&evnt)) //get and process events
	{
		switch (evnt.type)
		{
		case SDL_MOUSEWHEEL:
			myCamera.MoveBack(evnt.wheel.y);
			break;
		default:
			break;
		case SDL_MOUSEBUTTONDOWN:
			switch (evnt.button.button)
			{
			case SDL_BUTTON_LEFT:
				//SDL_ShowSimpleMessageBox(0, "Mouse", "Left button was pressed!", _gameDisplay.getWindow());
				fireMissiles(playerShip);
				break;
			case SDL_BUTTON_RIGHT:
				//SDL_ShowSimpleMessageBox(0, "Mouse", "Right button was pressed!", _gameDisplay.getWindow());
				break;
			case SDL_BUTTON_MIDDLE:
				break;
			default:
				//SDL_ShowSimpleMessageBox(0, "Mouse", "Some other button was pressed!", window);
				break;
			}
		case SDL_KEYDOWN:
			/* Check the SDLKey values and move change the coords */
			switch (evnt.key.keysym.sym)
			{
			case SDLK_a:
				playerShip.transformPositions(glm::vec3(*playerShip.getTM().GetPos()), glm::vec3(playerShip.getTM().GetRot()->x, playerShip.getTM().GetRot()->y, playerShip.getTM().GetRot()->z - 5 * deltaTime), glm::vec3(*playerShip.getTM().GetScale()));
				playerShip.turnObject(playerShip.getAngle() + 5 * deltaTime);
				break;
			case SDLK_w:
				playerShip.moveForward(30*deltaTime);
				break;
			case SDLK_s:
				playerShip.moveBack(20 * deltaTime);
				break;
			case SDLK_d:
				playerShip.transformPositions(glm::vec3(*playerShip.getTM().GetPos()), glm::vec3(playerShip.getTM().GetRot()->x, playerShip.getTM().GetRot()->y, playerShip.getTM().GetRot()->z + 5 * deltaTime), glm::vec3(*playerShip.getTM().GetScale()));
				playerShip.turnObject(playerShip.getAngle() - 5 * deltaTime);
				break;
			case SDLK_LEFT:
				myCamera.MoveLeft(10.0f*deltaTime);
				//cout << myCamera.getPos().x;
				break;
			case SDLK_RIGHT:
				myCamera.MoveRight(10.0f*deltaTime);
				break;
			case SDLK_UP:
				myCamera.MoveUp(10.0f*deltaTime);
				break;
			case SDLK_DOWN:
				myCamera.MoveDown(10.0f * deltaTime);
				break;
			case SDLK_SPACE:
				if (look)
					look = false;
				else
					look = true;
				break;
			case SDLK_BACKSPACE:
				if (shake)
					shake = false;
				else
					shake = true;
				break;
			default:
				break;
			case SDL_QUIT:
				_gameState = GameState::EXIT;
				break;
			}
		}

	}
}

void MainGame::initModels(GameObject*& asteroid)
{
	for (int i = 0; i < 20; ++i)
	{
		float rX = -1.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0 - -1.0)));
		float rY= -1.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0 - -1.0)));
		float rZ = -1.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0 - -1.0)));

		asteroid[i].transformPositions(glm::vec3(2.0 * i * rX, 2.0 * i * rY, 2.0 * i * rX), glm::vec3(rX, rY, rZ), glm::vec3(1.1, 1.1, 1.1));
		asteroid[i].update(&rockMesh);		
	}

	playerShip.transformPositions(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.2,0.2,0.2));
	playerShip.update(&shipMesh);

	aiShip.transformPositions(glm::vec3(0.0, 0.0, 80.0), glm::vec3(0.0, 180.0, 0.0), glm::vec3(0.2, 0.2, 0.2));
	aiShip.update(&shipMesh);

	planet.transformPositions(glm::vec3(0.0, 0.0, 40.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(20.0, 20.0, 20.0));
	planet.update(&planetMesh);

	for (int i = 0; i < 20; ++i)
	{
		missiles[i].setActive(0);
	}
}

void MainGame::drawPlanet()
{
	pTexture.Bind(0);
	basicTexture.Bind();
	basicTexture.Update(planet.getTM(), myCamera);
	planet.transformPositions(glm::vec3(*planet.getTM().GetPos()), glm::vec3(planet.getTM().GetRot()->x, planet.getTM().GetRot()->y + 0.1f*deltaTime, planet.getTM().GetRot()->z), glm::vec3(*planet.getTM().GetScale()));
	planet.draw(&planetMesh);
	planet.update(&planetMesh);

	
}

void MainGame::drawAsteriods()
{
	bTexture.Bind(0);
	eMapping.Bind();
	linkEmapping();

	for (int i = 0; i < 20; ++i)
	{
		if (asteroid[i].getActive())
		{
			asteroid[i].transformPositions(glm::vec3(*asteroid[i].getTM().GetPos()), glm::vec3(asteroid[i].getTM().GetRot()->x + deltaTime, asteroid[i].getTM().GetRot()->y + deltaTime, asteroid[i].getTM().GetRot()->z + deltaTime), glm::vec3(0.1, 0.1, 0.1));
			asteroid[i].draw(&rockMesh);
			asteroid[i].update(&rockMesh);
			eMapping.Update(asteroid[i].getTM(), myCamera);
		}
	}
}

void MainGame::drawMissiles()
{
	bTexture.Bind(0);
	rimShader.Bind();
	linkRimLighting();

	for (int i = 0; i < 20; ++i)
	{
		if (missiles[i].getActive())
		{
			missiles[i].draw(&missileMesh);
			missiles[i].update(&missileMesh);
			missiles[i].moveForward(10 * deltaTime);
			missiles[i].orbit(*planet.getTM().GetPos());
			rimShader.Update(missiles[i].getTM(), myCamera);
		}
	}
}

void MainGame::fireMissiles(GameObject ship) 
{
	// Set missile transform to ships position and set forward equal to ships current forward
	missiles[lastMissileFired].transformPositions(glm::vec3(*ship.getTM().GetPos()), glm::vec3(*ship.getTM().GetRot()), glm::vec3(0.05, 0.05, 0.05));
	missiles[lastMissileFired].setForward(ship.getForward());
	// Set missile to active
	missiles[lastMissileFired].setActive(true);
	
	// Check number of missiles is within 20
	if (lastMissileFired > 20)
	{
		lastMissileFired = 0;
	}
	else
	{
		lastMissileFired += 1 ;
	}
}

void MainGame::drawPlayerShip()
{
	toonShader.Bind();
	linkToon();

	playerShip.draw(&shipMesh);
	playerShip.update(&shipMesh);
	playerShip.orbit(*planet.getTM().GetPos());
	toonShader.Update(playerShip.getTM(), myCamera);
}

void MainGame::drawAIShip()
{
	toonShader.Bind();
	linkToon();

	aiShip.draw(&shipMesh);
	aiShip.update(&shipMesh);
	toonShader.Update(aiShip.getTM(), myCamera);
}


void MainGame::drawSkyBox()
{
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.textureID);

	counter = counter + 0.02f;

	skybox.draw(&myCamera);

	myCamera.setPos(currentCamPos);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnd();
}


bool MainGame::collision(glm::vec3 m1Pos, float m1Rad, glm::vec3 m2Pos, float m2Rad)
{
	float distance = glm::sqrt((m2Pos.x - m1Pos.x)*(m2Pos.x - m1Pos.x) + (m2Pos.y - m1Pos.y)*(m2Pos.y - m1Pos.y) + (m2Pos.z - m1Pos.z)*(m2Pos.z - m1Pos.z));

	if (distance < (m1Rad + m2Rad))
	{
		return true;
	}
	else
	{
		return false;
	}
}

//void MainGame::playAudio(unsigned int Source, glm::vec3 pos)
//{
//	
//	ALint state; 
//	alGetSourcei(Source, AL_SOURCE_STATE, &state);
//	/*
//	Possible values of state
//	AL_INITIAL
//	AL_STOPPED
//	AL_PLAYING
//	AL_PAUSED
//	*/
//	if (AL_PLAYING != state)
//	{
//		audioDevice.playSound(Source, pos);
//	}
//}

void MainGame::linkFogShader()
{
	//fogShader.setMat4("u_pm", myCamera.getProjection());
	//fogShader.setMat4("u_vm", myCamera.getProjection());
	fogShader.setFloat("maxDist", 20.0f);
	fogShader.setFloat("minDist", 0.0f);
	fogShader.setVec3("fogColor", glm::vec3(0.0f, 0.0f, 0.0f));
}

void MainGame::linkToon()
{
	toonShader.setVec3("lightDir", glm::vec3(0.5f, 0.5f, 0.5f));
}

void MainGame::linkGeo()
{
	float randX = ((float)rand() / (RAND_MAX));
	float randY = ((float)rand() / (RAND_MAX));
	float randZ = ((float)rand() / (RAND_MAX));
	// Frag: uniform float randColourX; uniform float randColourY; uniform float randColourZ;
	geoShader.setFloat("randColourX", randX);
	geoShader.setFloat("randColourY", randY);
	geoShader.setFloat("randColourZ", randZ);
	// Geom: uniform float time;
	geoShader.setFloat("time", counter);
}

void MainGame::linkRimLighting()
{
	glm::vec3 camDir;
	camDir = shipMesh.getSpherePos() - myCamera.getPos();
	camDir = glm::normalize(camDir);
	rimShader.setMat4("u_pm", myCamera.getProjection());
	rimShader.setMat4("u_vm", myCamera.getView());
	rimShader.setMat4("model", transform.GetModel());
	rimShader.setMat4("view", myCamera.getView());
	rimShader.setVec3("lightDir", glm::vec3(0.5f, 0.5f, 0.5f));
}

void MainGame::linkEmapping()
{
	eMapping.setMat4("model", asteroid[0].getModel());
	eMapping.setVec3("cameraPos", myCamera.getPos());
}

void MainGame::bindFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
}

void MainGame::unbindFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void MainGame::generateFBO(float w, float h)
{
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// create a colorbuffer for attachment texture
	glGenTextures(1, &CBO);
	glBindTexture(GL_TEXTURE_2D, CBO);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _gameDisplay.getWidth(), _gameDisplay.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, CBO, 0);

	// create a renderbuffer object for depth and stencil attachment 
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h); // use a single renderbuffer object for both a depth AND stencil buffer.
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO); // now actually attach it


// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		cout << "FRAMEBUFFER:: Framebuffer is complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MainGame::renderFBO()
{
	
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
	glClear(GL_COLOR_BUFFER_BIT);

	FBOShader.Bind();
	if(shake)
	FBOShader.setFloat("time", counter);
	else
	FBOShader.setFloat("time", 1);
	glBindVertexArray(quadVAO);
	glBindTexture(GL_TEXTURE_2D, CBO);	// use the color attachment texture as the texture of the quad plane
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void MainGame::runAI()
{
	// AI State Machine Code
	// If AI is in the TRACK state
	if (_aiState == AIState::TRACK)
	{
		// Move the AI towards the Player
		aiShip.setForward(glm::vec3(glm::normalize(playerShip.getTM().GetPos() - aiShip.getTM().GetPos())));
		aiShip.moveForward(1*deltaTime);

		// If the AI gets within a set range of the player
		float distance = glm::sqrt((aiShip.getTM().GetPos()->x - playerShip.getTM().GetPos()->x) * (aiShip.getTM().GetPos()->x - playerShip.getTM().GetPos()->x) + (aiShip.getTM().GetPos()->y - playerShip.getTM().GetPos()->y) * (aiShip.getTM().GetPos()->y - playerShip.getTM().GetPos()->y) + (aiShip.getTM().GetPos()->z - playerShip.getTM().GetPos()->z) * (aiShip.getTM().GetPos()->z - playerShip.getTM().GetPos()->z));
		if (distance < 30)
		{
			// Set state to ATTACK
			_aiState = AIState::ATTACK;
		}
	}
	// Else if AI is in ATTACK state
	else if (_aiState == AIState::ATTACK)
	{
		// Turn AI to face towards the player ship
		aiShip.setForward(glm::vec3(glm::normalize(playerShip.getTM().GetPos() - aiShip.getTM().GetPos())));
		fireMissiles(aiShip);

		// If the AI gets out of a set range of the player
		float distance = glm::sqrt((aiShip.getTM().GetPos()->x - playerShip.getTM().GetPos()->x) * (aiShip.getTM().GetPos()->x - playerShip.getTM().GetPos()->x) + (aiShip.getTM().GetPos()->y - playerShip.getTM().GetPos()->y) * (aiShip.getTM().GetPos()->y - playerShip.getTM().GetPos()->y) + (aiShip.getTM().GetPos()->z - playerShip.getTM().GetPos()->z) * (aiShip.getTM().GetPos()->z - playerShip.getTM().GetPos()->z));
		if (distance > 30)
		{
			// Set state to TRACK
			_aiState = AIState::TRACK;
		}
	}
}

void MainGame::drawGame()
{
	_gameDisplay.clearDisplay(0.8f, 0.8f, 0.8f, 1.0f); //sets our background colour	

	bindFBO();

	drawPlanet();
	drawAsteriods();
	drawPlayerShip();
	drawAIShip();
	drawSkyBox();
	drawMissiles();

	unbindFBO();

	renderFBO();

	glEnable(GL_DEPTH_TEST);

	/*drawPlanet();
	drawAsteriods();
	drawShip();
	drawSkyBox();
	drawMissiles();*/
	
	_gameDisplay.swapBuffer();		

} 

