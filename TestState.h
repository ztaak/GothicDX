#pragma once
#include "GameState.h"
#include "Camera.h"


class TestState : public GameState
{
public:
	TestState();
	~TestState();

public:
	virtual void onEnter();
	virtual void onExit();
	virtual void onPause();
	virtual void onUnPause();
	virtual void render();
	virtual void update();
private:
	Camera* mCamera;
};