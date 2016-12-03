#include "TestState.h"
#include "Core.h"
#include "ObjectManager.h"
TestState::TestState()
{
}

TestState::~TestState()
{
}


void TestState::onEnter()
{
	mCamera = new Camera();
	mCamera->setUp({0.0f, 0.0f, -8.0f}, 0.01f, 100.0f, GDX_WIDTH, GDX_HEIGHT, 45.0f);
	mCamera->sendToShader();
	GDX_CORE.loadTexture("dirt.jpg");
	ObjectManager::useGroup("test");
	ObjectManager::createNewAndInitialize("1", GDX_CORE.loadMesh(), GDX_DYNAMIC_OBJ, "dirt.jpg");
	ObjectManager::createNewAndInitialize("2", GDX_CORE.loadMesh(), GDX_DYNAMIC_OBJ);
	OB("1")->move({2.5f, 0.0f, 0.5f});
}
void TestState::onExit()
{
	printf("TEST ON EXIT\n");
}
void TestState::onPause()
{
	printf("TEST ON PAUSE\n");
}
void TestState::onUnPause()
{
	printf("TEST ON UN_PAUSE\n");
}
void TestState::render()
{
	ObjectManager::renderGroup("test");
}
void TestState::update()
{
	OB("2")->rotateZ(0.02f);
	if (GetAsyncKeyState('W')) mCamera->move(0.4f);
	if (GetAsyncKeyState('S')) mCamera->move(-0.4f);
	if (GetAsyncKeyState('D')) mCamera->strafe(-0.4f);
	if (GetAsyncKeyState('A')) mCamera->strafe(0.4f);
	if (GetAsyncKeyState(VK_UP)) mCamera->pitch(2.5f);
	if (GetAsyncKeyState(VK_DOWN)) mCamera->pitch(-2.5f);
	if (GetAsyncKeyState(VK_RIGHT)) mCamera->yaw(-2.5f);
	if (GetAsyncKeyState(VK_LEFT)) mCamera->yaw(2.5f);
	mCamera->update();
	ObjectManager::updateGroup("test");
}