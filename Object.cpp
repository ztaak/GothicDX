#include "Object.h"


Object::Object()
{
	initialized = false;
}

Object::Object(std::string tag, std::string group)
{
	mTag = tag;
	mGroup = group;
	initialized = false;
}

void Object::init(SMesh * mesh, GDX_OBJECT_TYPE type)
{
	mObjectType = type;
	initialize(mesh);
	XMStoreFloat4x4(&mWorldMatrix, XMMatrixIdentity());
	setMetrices({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f});

	GDX_CORE.loadTexture("dirt.jpg");
	mTexture = GDX_CORE.getTexture("dirt.jpg");

	recalc = true;
	initialized = true;
}

void Object::setTag(std::string tag)
{
	mTag = tag;
}

void Object::setGroup(std::string group)
{
	mGroup = group;
}

void Object::setMetrices(XMFLOAT3 pos, XMFLOAT3 scale, XMFLOAT3 rotation)
{
	setPostition(pos);
	setScale(scale);
	setRotationX(rotation.x);
	setRotationY(rotation.y);
	setRotationZ(rotation.z);
	recalc = true;
}

void Object::setPostition(XMFLOAT3 pos)
{
	mPos = pos;
	recalc = true;
}

void Object::setScale(XMFLOAT3 scale)
{
	mScale = scale;
	recalc = true;
}

void Object::setRotationX(float v)
{
	mRotation.x = v;
	recalc = true;
}

void Object::setRotationY(float v)
{
	mRotation.y = v;
	recalc = true;
}

void Object::setRotationZ(float v)
{
	mRotation.z = v;
	recalc = true;
}

void Object::move(XMFLOAT3 v)
{
	mPos.x += v.x;
	mPos.y += v.y;
	mPos.z += v.z;
	recalc = true;
}

void Object::scale(XMFLOAT3 v)
{
	mScale.x = v.x;
	mScale.y = v.y;
	mScale.z = v.z;
	recalc = true;
}

void Object::rotateX(float v)
{
	mRotation.x += v;
	recalc = true;
}

void Object::rotateY(float v)
{
	mRotation.y += v;
	recalc = true;
}

void Object::rotateZ(float v)
{
	mRotation.z += v;
	recalc = true;
}

void Object::draw()
{
	if (!initialized)
		LOG::DEB("Trying to draw object: '%s' in group: '%s' without initialization.", mTag, mGroup);
	else {
		SBPerObject spObj;
		ZeroMemory(&spObj, sizeof(spObj));

		spObj.worldMatrix = mWorldMatrix;
		GDX_CORE.updateObjectConstantBuffer(&spObj);
		GDX_CORE.bindTexture(&mTexture);
		renderBuffer();
	}
}

void Object::update()
{
	if (!initialized)
		LOG::DEB("Trying to use object: '%s' in group: '%s' without initialization.", mTag, mGroup);
	else {
		if ((mObjectType == GDX_DYNAMIC_OBJ || mObjectType == GDX_DEFAULT_OBJ) && recalc)
		{
			XMMATRIX temp = XMMatrixIdentity();
			temp = XMMatrixMultiply(temp, XMMatrixRotationX(mRotation.x));
			temp = XMMatrixMultiply(temp, XMMatrixRotationY(mRotation.y));
			temp = XMMatrixMultiply(temp, XMMatrixRotationZ(mRotation.z));
			temp = XMMatrixMultiply(temp, XMMatrixScaling(mScale.x, mScale.y, mScale.z));
			temp = XMMatrixMultiply(temp, XMMatrixTranslation(mPos.x, mPos.y, mPos.z));
			XMStoreFloat4x4(&mWorldMatrix, temp);
			recalc = false;
		}
	}
}


Object::~Object()
{
}
