#include "DeltaTime.h"
#include <iostream>
#include <string>

using namespace std;

DeltaTime::DeltaTime()
{

}

DeltaTime::~DeltaTime()
{

}

float DeltaTime::calculateDeltaTime(bool usingDeltaTime, float overrideTime)
{
	if (usingDeltaTime)
	{
		secondFrameTime = firstFrameTime;
		firstFrameTime = SDL_GetPerformanceCounter();

		deltaTime = (double)((firstFrameTime - secondFrameTime) / (double)SDL_GetPerformanceFrequency());
		//cout << "Delta time equals: " << deltaTime;		
	}
	else
	{
		deltaTime = overrideTime;
	}
	return deltaTime;
}