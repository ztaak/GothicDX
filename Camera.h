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
	void move(float v);
	void strafe(float v);
	void yaw(float v);
	void pitch(float v);
	void roll(float v);
	void sendToShader();
	~Camera();
private:
	float mYaw;
	float mPitch;
	float mRoll;

	XMFLOAT3 pCameraPosition;
	XMFLOAT3 pCameraTarget;
	XMFLOAT3 pUpVector;
	XMFLOAT3 pRightVector;

	XMFLOAT4X4 mProjMatrix;
	XMFLOAT4X4 mViewMatrix;
};

