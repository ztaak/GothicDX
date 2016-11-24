#pragma once

#include <DirectXMath.h>
#include <D3DX11.h>
#include <D3D11.h>
#include <D3DX10.h>
#include <DXGI.h>

using namespace DirectX;

class Camera
{
public:
	Camera();
	void setUp(XMFLOAT3 pos, float pNear, float pFar, float width, float height, float fov);
	void update();
	void sendToShader();
	~Camera();
private:
	XMFLOAT4X4 mProjMatrix;
	XMFLOAT4X4 mViewMatrix;
};

