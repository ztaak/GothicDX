#pragma once

#include <DirectXMath.h>
#include <vector>

struct SVertex
{
	SVertex() {}
	SVertex(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 col) : position(pos), color(col) {}
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 color;
};

struct SMesh 
{
	std::vector<SVertex> vertices;
	std::vector<UINT> indices;
};

