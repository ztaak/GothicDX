#pragma once

#include <D3DX11.h>
#include <D3D11.h>
#include <D3DX10.h>
#include <DXGI.h>
#include "Mesh.h"

class Buffer
{
public:
	Buffer();
	~Buffer();
	void initialize(SMesh *mesh);
	void renderBuffer();
protected:
	ID3D11Buffer* mpVertexBuffer;
	ID3D11Buffer* mpIndexBuffer;
	UINT vertsToDraw;
};

