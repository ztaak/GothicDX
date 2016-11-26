#pragma once

#include <string>
#include <stdio.h>

class GameState
{
public:
	GameState() {}
	virtual void render() = 0;
	virtual void update() = 0;
	virtual void onEnter() = 0;
	virtual void onExit() = 0;
	virtual void onPause() = 0;
	virtual void onUnPause() = 0;

	const std::string& getStateId() const { return stateId; }

	~GameState() {}

protected:
	std::string stateId;

};