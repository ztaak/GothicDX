

#include "Core.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SWindowInfo wi;
	ZeroMemory(&wi, sizeof(wi));
	wi.x = CW_USEDEFAULT;
	wi.y = CW_USEDEFAULT;
	wi.renderWidth = 1366.0f;
	wi.renderHeight = 768.0f;

	if (FAILED(Core::instance().createWindow(hInstance, wi))) return -1;
	if (FAILED(Core::instance().initDirectX11())) return -1;
	return Core::instance().loop();
}