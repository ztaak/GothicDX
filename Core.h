#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include <D3DX11.h>
#include <D3D11.h>
#include <D3DX10.h>
#include <DXGI.h>
#include <IL\il.h>
#include <IL\ilu.h>
#include <map>
#include "LOG.h"
#include "Mesh.h"
#include "Camera.h"
#include "TestState.h"

#define GDX_WIDTH Core::instance().getSWindowInfo().renderWidth
#define GDX_HEIGHT Core::instance().getSWindowInfo().renderHeight
#define GDX_CORE Core::instance()

typedef unsigned int GDX_SHADER;
typedef ID3D11ShaderResourceView* GDX_TEXTURE;

using namespace DirectX;

static GDX_SHADER DEFAULT_SHADER_ID;

struct SWindowInfo {
	float x;
	float y;
	float renderWidth;
	float renderHeight;
	bool fullscreen;
	UINT antialiasing;
	UINT aaQuality;
};

struct SShader
{
	ID3D11VertexShader* vertex;
	ID3D11PixelShader* pixel;
	ID3D11InputLayout* inputLayout;
};

struct SBPerApp 
{
	DirectX::XMFLOAT4X4 projectionMatrix;
};

struct SBPerFrame
{
	DirectX::XMFLOAT4X4 viewMatrix;
};

struct SBPerObject
{
	DirectX::XMFLOAT4X4 worldMatrix;
};

template<typename T>
HRESULT createConstantBuffer(D3D11_USAGE usage, UINT cpuAccessFlag, UINT index,
	ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11Buffer** bufferOut) 
{
	T bufferData;
	ZeroMemory(&bufferData, sizeof(T));

	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(T);
	cbDesc.Usage = usage;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = cpuAccessFlag;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &bufferData;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&cbDesc, &InitData,
		bufferOut);

	if (FAILED(hr))
		return hr;

	deviceContext->VSSetConstantBuffers(index, 1, bufferOut);
	deviceContext->PSSetConstantBuffers(index, 1, bufferOut);
	return S_OK;
}

class Core
{
public:
	static Core& instance() 
	{
		static Core INSTANCE;
		return INSTANCE;
	}
	HRESULT createWindow(HINSTANCE hinst, SWindowInfo wi);
	HRESULT initDirectX11();
	SWindowInfo getSWindowInfo() { return mWindowInfo; }
	ID3D11Device* getDevice() { return mDevice; }
	ID3D11DeviceContext* getDeviceContext() { return mDeviceContext; }
	HRESULT compileShader(UINT id, std::string pathToVertexShader, 
			std::string pathToPixelShader, D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc, UINT ildElements);
	GDX_SHADER genShader();
	HRESULT deleteShader(GDX_SHADER id);
	HRESULT bindShader(GDX_SHADER id);
	SMesh* loadMesh();
	HRESULT updateAppConstantBuffer(SBPerApp * pData);
	HRESULT updateFrameConstantBuffer(SBPerFrame * pData);
	HRESULT updateObjectConstantBuffer(SBPerObject * pData);
	void pushState(GameState* nState);
	void changeState(GameState* nState);
	void popState();
	GDX_TEXTURE getTexture(std::string path);
	HRESULT bindTexture(GDX_TEXTURE* texture);
	HRESULT loadTexture(std::string path);
	HRESULT unloadTexture(std::string path);
	int loop();
private:
	Core();

	HRESULT createDevice();
	HRESULT createSwapChain();
	HRESULT createTargetAndDepthView();
	HRESULT createViewPort();
	HRESULT createLayoutAndCompileShaders();
	HRESULT initializeConstantBuffers();
	HRESULT createAndUseDefaultSampler();

	ID3D11Buffer* mBufferPerApp;
	ID3D11Buffer* mBufferPerFrame;
	ID3D11Buffer* mBufferPerObject;

	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;
	ID3D11RenderTargetView* mRenderTargetView;
	ID3D11DepthStencilView* mDepthStencilView;
	IDXGISwapChain* mSwapChain;
	ID3D11SamplerState* mSamplerState;

	HWND mWindowHandle;
	HINSTANCE mInstanceHandle;
	SWindowInfo mWindowInfo;

	std::vector<GameState*> mGameStates;
	std::vector<SShader*> mShaders;
	std::map<std::string, GDX_TEXTURE> mTextures;


	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_CLOSE:
			DestroyWindow(hwnd);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}

		return 0;
	}

};

