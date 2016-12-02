#pragma once

#include <DirectXMath.h>
#include <vector>

struct SVertex
{
	SVertex() {}
	SVertex(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT2 cor) : position(pos), coord(cor) {}
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 coord;
};

struct SMesh 
{
	std::vector<SVertex> vertices;
	std::vector<UINT> indices;
};

