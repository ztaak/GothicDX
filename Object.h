#pragma once

#include "Buffer.h"
#include <DirectXMath.h>

enum GDX_OBJECT_TYPE 
{
	GDX_DYNAMIC_OBJ,
	GDX_STATIC_OBJ,
	GDX_DEFAULT_OBJ
};

using namespace DirectX;

class Object : public Buffer
{
public:
	Object();
	void init(SMesh* mesh, GDX_OBJECT_TYPE type);

	void setMetrices(XMFLOAT3 pos, XMFLOAT3 scale, XMFLOAT3 rotation);
	void setPostition(XMFLOAT3 pos);
	void setScale(XMFLOAT3 scale);
	void setRotationX(float v);
	void setRotationY(float v);
	void setRotationZ(float v);

	void move(XMFLOAT3 v);
	void scale(XMFLOAT3 v);
	void rotateX(float v);
	void rotateY(float v);
	void rotateZ(float v);

	void draw();
	void update();
	~Object();
private:
	XMFLOAT4X4 mWorldMatrix;
	XMFLOAT3 mPos;
	XMFLOAT3 mScale;
	XMFLOAT3 mRotation;
	bool recalc;
	GDX_OBJECT_TYPE mObjectType;
};

