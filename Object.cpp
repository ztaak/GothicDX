#include "Object.h"
#include "Core.h"


Object::Object()
{
}

void Object::init(SMesh * mesh, GDX_OBJECT_TYPE type)
{
	mObjectType = type;
	initialize(mesh);
	XMStoreFloat4x4(&mWorldMatrix, DirectX::XMMatrixIdentity());
}

void Object::draw()
{
	renderBuffer();
}

void Object::update()
{
	SBPerObject spObj;
	ZeroMemory(&spObj, sizeof(spObj));

	if (mObjectType == GDX_DYNAMIC_OBJ || mObjectType == GDX_DEFAULT_OBJ)
	{
		XMMATRIX temp = DirectX::XMMatrixIdentity();
		temp = DirectX::XMMatrixTranslation(-1.0f, 0.0f, 0.0f);
		//temp = XMMatrixScaling(2.0f, 5.0f, 1.0f);
		//temp = XMMatrixTranspose(temp);
		XMStoreFloat4x4(&mWorldMatrix, temp);
	}

	spObj.worldMatrix = mWorldMatrix;
	Core::instance().updateObjectConstantBuffer(&spObj);
}


Object::~Object()
{
}
