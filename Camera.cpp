#include "Camera.h"

#include "Core.h"

Camera::Camera()
{
}

void Camera::setUp(XMFLOAT3 pos, float pNear, float pFar, float width, float height, float fov)
{
	XMMATRIX tempProjMatrix = XMMatrixPerspectiveFovLH(fov, width / height, pNear, pFar);
	XMStoreFloat4x4(&mProjMatrix, tempProjMatrix);

	XMVECTOR tpos = XMVectorSet(pos.x, pos.y, pos.z, 1.0f);
	XMVECTOR target = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR right = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	mYaw = 90.0f;
	mPitch = 0.0f;
	mRoll = 0.0f;

	XMStoreFloat3(&pCameraPosition, tpos);
	XMStoreFloat3(&pCameraTarget, target);
	XMStoreFloat3(&pUpVector, up);
	XMStoreFloat3(&pRightVector, right);

	XMMATRIX V = XMMatrixLookAtLH(tpos, tpos + target, up);
	XMStoreFloat4x4(&mViewMatrix, V);
}

void Camera::update()
{
	XMFLOAT3 front;
	front.x = cos(XMConvertToRadians(mPitch)) * cos(XMConvertToRadians(mYaw));
	front.y = sin(XMConvertToRadians(mPitch));
	front.z = cos(XMConvertToRadians(mPitch)) * sin(XMConvertToRadians(mYaw));
	XMVECTOR vecUpVector = XMLoadFloat3(&pUpVector);
	XMVECTOR vecCameraTarget =  XMVector3Normalize(XMLoadFloat3(&front));
	XMVECTOR vecRightVector = XMVector3Normalize(XMVector3Cross(vecCameraTarget, vecUpVector));
	XMVECTOR vecCameraPosition = XMLoadFloat3(&pCameraPosition);
	XMMATRIX V = XMMatrixLookAtLH(vecCameraPosition, vecCameraPosition + vecCameraTarget, vecUpVector);
	XMStoreFloat4x4(&mViewMatrix, V);

	XMStoreFloat3(&pCameraTarget, vecCameraTarget);
	XMStoreFloat3(&pCameraPosition, vecCameraPosition);
	XMStoreFloat3(&pRightVector, vecRightVector);

	SBPerFrame cbPerFrame;
	ZeroMemory(&cbPerFrame, sizeof(cbPerFrame));
	cbPerFrame.viewMatrix = mViewMatrix;
	Core::instance().updateFrameConstantBuffer(&cbPerFrame);
}

void Camera::move(float v)
{
	XMVECTOR V = XMLoadFloat3(&pCameraPosition) + (v * XMLoadFloat3(&pCameraTarget));
	XMStoreFloat3(&pCameraPosition, V);
}

void Camera::strafe(float v)
{
	XMVECTOR V = XMLoadFloat3(&pCameraPosition) + (v * XMLoadFloat3(&pRightVector));
	XMStoreFloat3(&pCameraPosition, V);
}

void Camera::yaw(float v)
{
	mYaw += v;
}

void Camera::pitch(float v)
{
	mPitch += v;
	if (mPitch > 89.0f)
		mPitch = 89.0f;
	if (mPitch < -89.0f)
		mPitch = -89.0f;
}

void Camera::roll(float v)
{
	mRoll += v;
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
