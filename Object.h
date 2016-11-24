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
	void draw();
	void update();
	~Object();
private:
	DirectX::XMFLOAT4X4 mWorldMatrix;
	GDX_OBJECT_TYPE mObjectType;
};

