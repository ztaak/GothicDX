#include "Camera.h"

#include "Core.h"

Camera::Camera()
{
}

void Camera::setUp(XMFLOAT3 pos, float pNear, float pFar, float width, float height, float fov)
{
	XMMATRIX tempProjMatrix = XMMatrixPerspectiveFovLH(fov, width / height, pNear, pFar);
	//tempProjMatrix = XMMatrixTranspose(tempProjMatrix);
	XMStoreFloat4x4(&mProjMatrix, tempProjMatrix);

	XMVECTOR tpos = XMVectorSet(0.0f, 0.0f, -8.0f, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(tpos, target, up);
	//V = XMMatrixTranspose(V);
	XMStoreFloat4x4(&mViewMatrix, V);
}

void Camera::update()
{
	SBPerFrame cbPerFrame;
	ZeroMemory(&cbPerFrame, sizeof(cbPerFrame));
	cbPerFrame.viewMatrix = mViewMatrix;
	Core::instance().updateFrameConstantBuffer(&cbPerFrame);
}

void Camera::sendToShader()
{
	SBPerApp cbPerApp;
	ZeroMemory(&cbPerApp, sizeof(cbPerApp));
	cbPerApp.projectionMatrix = mProjMatrix;
	Core::instance().updateAppConstantBuffer(&cbPerApp);

	SBPerFrame cbPerFrame;
	ZeroMemory(&cbPerFrame, sizeof(cbPerFrame));
	cbPerFrame.viewMatrix = mViewMatrix;
	Core::instance().updateFrameConstantBuffer(&cbPerFrame);
}


Camera::~Camera()
{
}
