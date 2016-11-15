#include "Buffer.h"
#include "Core.h"


Buffer::Buffer()
{
}


Buffer::~Buffer()
{
}

void Buffer::initialize(SMesh * mesh)
{
	vertsToDraw = mesh->indices.size();
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SVertex) * mesh->vertices.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = &mesh->vertices[0];
	HRESULT hr = Core::instance().getDevice()->CreateBuffer(&bd, &InitData, &mpVertexBuffer);
	if (FAILED(hr)) LOG::ERR("Creating vertex buffer failed.");


	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(UINT) * mesh->indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA iInitData;
	ZeroMemory(&iInitData, sizeof(iInitData));
	iInitData.pSysMem = &mesh->indices[0];
	hr = Core::instance().getDevice()->CreateBuffer(&ibd, &iInitData, &mpIndexBuffer);
	if (FAILED(hr)) LOG::ERR("Creating index buffer failed.");
}

void Buffer::renderBuffer()
{
	UINT stride = sizeof(SVertex);
	UINT offset = 0;


	Core::instance().bindShader(DEFAULT_SHADER_ID);
	Core::instance().getDeviceContext()->IASetVertexBuffers(0, 1, &mpVertexBuffer, &stride, &offset);
	Core::instance().getDeviceContext()->IASetIndexBuffer(mpIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	Core::instance().getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Core::instance().getDeviceContext()->DrawIndexed(vertsToDraw, 0, 0);
}
