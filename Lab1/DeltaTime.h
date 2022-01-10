#pragma once
#include <SDL\SDL.h>
#include <GL/glew.h>
#include <iostream>
#include <string>

class DeltaTime
{
public:

	DeltaTime();
	~DeltaTime();

	float deltaTime = 0;

	float calculateDeltaTime(bool usingDeltaTime, float overrideTime);

private:



	Uint64 firstFrameTime = SDL_GetPerformanceCounter();
	Uint64 secondFrameTime = 0;
}; 