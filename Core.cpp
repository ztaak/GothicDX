#include "Core.h"


HRESULT Core::createWindow(HINSTANCE hinst, SWindowInfo wi)
{
	mWindowInfo = wi;
	mInstanceHandle = hinst;

	LPWSTR windowClassName = L"class";

	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = (WNDPROC)Core::WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mInstanceHandle;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = windowClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc)){
		LOG::ERR("Register window class.");
		return E_FAIL;
	}
	RECT rc = { 0, 0, mWindowInfo.renderWidth, mWindowInfo.renderHeight };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	mWindowHandle = CreateWindowEx(WS_EX_APPWINDOW, windowClassName, L"GothicDX", WS_OVERLAPPEDWINDOW,
		mWindowInfo.x, mWindowInfo.y, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, mInstanceHandle, NULL);

	if (mWindowHandle == NULL){
		LOG::ERR("Crearing window handle failed.");
		return E_FAIL;
	}

	ShowWindow(mWindowHandle, true); 
	UpdateWindow(mWindowHandle);

	return S_OK;
}

HRESULT Core::initDirectX11()
{
	if (FAILED(createDevice())) {
		LOG::ERR("Initailizing DirectX11 failed while creating device.");
		return E_FAIL;
	}
	if (FAILED(createSwapChain())) {
		LOG::ERR("Initailizing DirectX11 failed while creating swap chain.");
		return E_FAIL;
	}
	if (FAILED(createTargetAndDepthView())) {
		LOG::ERR("Initailizing DirectX11 failed while creating render target view and depth view.");
		return E_FAIL;
	}
	if (FAILED(createViewPort())) {
		LOG::ERR("Initailizing DirectX11 failed while creating view port.");
		return E_FAIL;
	}
	if (FAILED(createLayoutAndCompileShaders())) {
		LOG::ERR("Initailizing DirectX11 failed while creating input layout or compiling shaders.");
		return E_FAIL;
	}
	if (FAILED(initializeConstantBuffers())) {
		LOG::ERR("Initailizing constant buffers failed!");
		return E_FAIL;
	}
	if (FAILED(createAndUseDefaultSampler())) {
		LOG::ERR("Creating default sampler failed!");
		return E_FAIL;
	}
	ilInit();
	iluInit();
	return S_OK;
}

HRESULT Core::compileShader(UINT id, std::string pathToVertexShader,
							std::string pathToPixelShader, D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc, UINT ildElements)
{

	UINT flags = 0;
#ifdef _DEBUG
	flags |= D3D10_SHADER_DEBUG;
#endif

	ID3D10Blob* vertBlob;
	ID3D10Blob* errBlob;

	HRESULT hr = D3DX11CompileFromFileA(pathToVertexShader.c_str(), 0, 0, "VS", "vs_4_0", flags, 0, 0, &vertBlob, &errBlob, 0);
	if (errBlob != 0) {
		LOG::ERR("Compiling vertex shader failed!\n%s", (char*)errBlob->GetBufferPointer());
		errBlob->Release();
		errBlob = NULL;
		return E_FAIL;
	}
	if (FAILED(hr)) {
		LOG::ERR("Compiling vertex shader failed! HRESULT error.");
		return hr;
	}

	ID3D11VertexShader* vertexShader;
	hr = mDevice->CreateVertexShader(vertBlob->GetBufferPointer(), vertBlob->GetBufferSize(), NULL, &vertexShader);
	if (FAILED(hr)) {
		LOG::ERR("Creating vertex shader failed!");
		errBlob->Release();
		errBlob = NULL;
		vertBlob->Release();
		vertBlob = NULL;
		vertexShader->Release();
		vertexShader = NULL;
		return hr;
	}
	ID3D11InputLayout* inputLayout;

	hr = mDevice->CreateInputLayout(inputLayoutDesc, ildElements, vertBlob->GetBufferPointer(), 
									vertBlob->GetBufferSize(), &inputLayout);

	vertBlob->Release();
	vertBlob = NULL;

	if (FAILED(hr)) {
		LOG::ERR("Creating input layout failed!");
		errBlob->Release();
		errBlob = NULL;
		vertexShader->Release();
		vertexShader = NULL;
		inputLayout->Release();
		inputLayout = NULL;
		return hr;
	}

	ID3D10Blob* pixBlob;

	hr = D3DX11CompileFromFileA(pathToPixelShader.c_str(), 0, 0, "PS", "ps_4_0", flags, 0, 0, &pixBlob, &errBlob, 0);
	if (errBlob != 0) {
		LOG::ERR("Compiling pixel shader failed!\n%s", (char*)errBlob->GetBufferPointer());
		errBlob->Release();
		errBlob = NULL;
		pixBlob->Release();
		pixBlob = NULL;
		return E_FAIL;
	}
	if (FAILED(hr)) {
		LOG::ERR("Compiling pixel shader failed!");
		return hr;
	}

	ID3D11PixelShader* pixelShader;
	hr = mDevice->CreatePixelShader(pixBlob->GetBufferPointer(), pixBlob->GetBufferSize(), NULL, &pixelShader);
	if (FAILED(hr)) {
		LOG::ERR("Creating pixel shader failed!");
		pixBlob->Release();
		pixBlob = NULL;
		pixelShader->Release();
		pixelShader = NULL;
		return hr;
	}
	pixBlob->Release();
	pixBlob = NULL;

	mShaders[id]->inputLayout = inputLayout;
	mShaders[id]->vertex = vertexShader;
	mShaders[id]->pixel = pixelShader;

	return S_OK;
}

GDX_SHADER Core::genShader()
{
	SShader* shaderAlloc = new SShader();
	mShaders.push_back(shaderAlloc);
	return mShaders.size() - 1;
}

HRESULT Core::deleteShader(GDX_SHADER id)
{
	return E_NOTIMPL;
}

HRESULT Core::bindShader(GDX_SHADER id)
{
	SShader* s = mShaders[id];
	if (s != nullptr) {
		mDeviceContext->IASetInputLayout(s->inputLayout);
		mDeviceContext->VSSetShader(s->vertex, 0, 0);
		mDeviceContext->PSSetShader(s->pixel, 0, 0);
		return S_OK;
	}
	return E_FAIL;
}

SMesh * Core::loadMesh()
{
	SMesh* mesh = new SMesh();
	mesh->vertices.push_back(SVertex({ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) }));
	mesh->vertices.push_back(SVertex({ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) }));
	mesh->vertices.push_back(SVertex({ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) }));
	mesh->vertices.push_back(SVertex({ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) }));

	mesh->vertices.push_back(SVertex({ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) }));
	mesh->vertices.push_back(SVertex({ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) }));
	mesh->vertices.push_back(SVertex({ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) }));
	mesh->vertices.push_back(SVertex({ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) }));

	mesh->vertices.push_back(SVertex({ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) }));
	mesh->vertices.push_back(SVertex({ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) }));
	mesh->vertices.push_back(SVertex({ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) }));
	mesh->vertices.push_back(SVertex({ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) }));

	mesh->vertices.push_back(SVertex({ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) }));
	mesh->vertices.push_back(SVertex({ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) }));
	mesh->vertices.push_back(SVertex({ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) }));
	mesh->vertices.push_back(SVertex({ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) }));

	mesh->vertices.push_back(SVertex({ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) }));
	mesh->vertices.push_back(SVertex({ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) }));
	mesh->vertices.push_back(SVertex({ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) }));
	mesh->vertices.push_back(SVertex({ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) }));

	mesh->vertices.push_back(SVertex({ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) }));
	mesh->vertices.push_back(SVertex({ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) }));
	mesh->vertices.push_back(SVertex({ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) }));
	mesh->vertices.push_back(SVertex({ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) }));
	UINT indicies[36] =
	{
		3,1,0,
		2,1,3,
		6,4,5,
		7,4,6,
		11,9,8,
		10,9,11,
		14,12,13,
		15,12,14,
		19,17,16,
		18,17,19,
		22,20,21,
		23,20,22
	};

	for (int i = 0; i < 36; ++i) 
	{
		mesh->indices.push_back(indicies[i]);
	}

	return mesh;
}

HRESULT Core::updateAppConstantBuffer(SBPerApp * pData)
{
	mDeviceContext->UpdateSubresource(mBufferPerApp, 0, 0, (SBPerApp*)pData, 0, 0);
	mDeviceContext->PSSetConstantBuffers(0, 1, &mBufferPerApp);

	return S_OK;
}

HRESULT Core::updateFrameConstantBuffer(SBPerFrame * pData)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	mDeviceContext->Map(mBufferPerFrame, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	SBPerFrame* dataPtr = (SBPerFrame*)mappedResource.pData;
	memcpy(dataPtr, pData, sizeof(SBPerFrame));

	mDeviceContext->Unmap(mBufferPerFrame, 0);
	return S_OK;
}

HRESULT Core::updateObjectConstantBuffer(SBPerObject * pData)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	mDeviceContext->Map(mBufferPerObject, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	SBPerObject* dataPtr = (SBPerObject*)mappedResource.pData;
	memcpy(dataPtr, pData, sizeof(SBPerObject));

	mDeviceContext->Unmap(mBufferPerObject, 0);
	return S_OK;
}

void Core::pushState(GameState* nState)
{
	if (mGameStates.size() > 0)
		mGameStates.back()->onPause();
	mGameStates.push_back(nState);
	mGameStates.back()->onEnter();
}

void Core::changeState(GameState* nState)
{
	popState();
	pushState(nState);
}

void Core::popState()
{
	if (mGameStates.size() > 1) {
		mGameStates.back()->onExit();
		mGameStates.pop_back();
		mGameStates.back()->onUnPause();
	}
	else if (mGameStates.size() > 0) {
		mGameStates.back()->onExit();
		mGameStates.pop_back();
	}
}

GDX_TEXTURE Core::getTexture(std::string path)
{
	if (mTextures[path] == nullptr) {
		LOG::DEB("Texture '%s' not loaded!", path.c_str());
		return nullptr;
		//loadTexture(path);
	}
	return mTextures[path];
}

HRESULT Core::bindTexture(GDX_TEXTURE* texture)
{
	mDeviceContext->PSSetShaderResources(0, 1, texture);
	return S_OK;
}

HRESULT Core::loadTexture(std::string path)
{
	if (mTextures[path] != nullptr) {
		//LOG::DEB("Texture '%s' already loaded!", path.c_str());
		return S_OK;
	}
	ILuint devilImage;
	ilGenImages(1, &devilImage);
	ilBindImage(devilImage);
	std::wstring wPath = std::wstring(path.begin(), path.end());
	ilEnable(IL_FORMAT_SET);
	ilSetInteger(IL_FORMAT_MODE, IL_RGBA);
	ilLoadImage(wPath.c_str());

	ILenum ilErr;
	ilErr = ilGetError();
	if (ilErr != IL_NO_ERROR) {
		LOG::DEB("Loading texture: '%s' failed! %d: %s", path.c_str(), ilErr, iluErrorString(ilErr));
		ilDeleteImages(1, &devilImage);
		return E_FAIL;
	}

	ILuint devilWidth, devilHeight;
	devilWidth = ilGetInteger(IL_IMAGE_WIDTH);
	devilHeight = ilGetInteger(IL_IMAGE_HEIGHT);
	
	ILuint devilBPP = ilGetInteger(IL_IMAGE_BPP);
	ILuint devilBitsPP = ilGetInteger(IL_IMAGE_BITS_PER_PIXEL);

	if (devilBPP != 4) {
		LOG::DEB("Wrong BPP in texture: '%s'", path.c_str());
		return E_FAIL;
	}
	if (devilBitsPP != 32) {
		LOG::DEB("Wrong bits per pixel in texture: '%s'", path.c_str());
		return E_FAIL;
	}

	ILubyte *devilPixels = ilGetData();


	//LOG::DEB("Width: %i, Height: %i, BPP: %i, BitsPerPixel: %i", devilWidth, devilHeight, devilBPP, devilBitsPP);
	D3D11_TEXTURE2D_DESC t2d;
	t2d.Width = (UINT)(devilWidth);
	t2d.Height = (UINT)(devilHeight);
	t2d.MipLevels = 1;
	t2d.ArraySize = 1;
	t2d.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	t2d.SampleDesc.Count = 1;
	t2d.SampleDesc.Quality = 0;
	t2d.Usage = D3D11_USAGE_DEFAULT;
	t2d.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	t2d.CPUAccessFlags = 0;
	t2d.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA sd;
	sd.pSysMem = (void *)(devilPixels);
	sd.SysMemPitch = devilWidth * devilBPP;
	sd.SysMemSlicePitch = devilWidth * devilHeight * devilBPP;

	ID3D11Texture2D* tempTexture;

	HRESULT hr = mDevice->CreateTexture2D(&t2d, &sd, &tempTexture);
	if (FAILED(hr)) {
		LOG::ERR("Creating Texture2D from texture '%s' failed!", path.c_str());
		ilDeleteImages(1, &devilImage);
		return E_FAIL;
	}

	ID3D11ShaderResourceView* srvTexture;
	hr = mDevice->CreateShaderResourceView(tempTexture, NULL, &srvTexture);
	if (FAILED(hr)){
		LOG::ERR("Can't create sahder resource view for texture: '%s'.", path.c_str());
		tempTexture->Release();
		tempTexture = nullptr;
		return E_FAIL;
	}
	tempTexture->Release();
	tempTexture = nullptr;
	ilDeleteImages(1, &devilImage);


	mTextures[path] = srvTexture;

	return S_OK;
}

HRESULT Core::unloadTexture(std::string path)
{
	return E_NOTIMPL;
}

int Core::loop()
{
	MSG msg = { 0 };
	
	pushState(new TestState());


	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; 
			mDeviceContext->ClearRenderTargetView(mRenderTargetView, ClearColor);
			mDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);


			if (mGameStates.size() > 0) {
				mGameStates.back()->update();
				mGameStates.back()->render();
			}

			mSwapChain->Present(1, 0);
		}
	}
	return (int)msg.wParam;
}

Core::Core()
{
}

HRESULT Core::createDevice()
{
	HRESULT hr = S_OK;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;

	hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, NULL, NULL, D3D11_SDK_VERSION,
							&mDevice, &featureLevel, &mDeviceContext);

	if (featureLevel != D3D_FEATURE_LEVEL_11_0) {
		LOG::ERR("DirectX 11 is not supported!");
		mDevice->Release();
		mDevice = NULL;
		mDeviceContext->Release();
		mDeviceContext = NULL;
		return E_FAIL;
	}

	if (FAILED(hr))
		return hr;
}

HRESULT Core::createSwapChain()
{	
	HRESULT hr = S_OK;
	mWindowInfo.aaQuality = 0;
	if (mWindowInfo.antialiasing > 1) {
		hr = mDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, mWindowInfo.antialiasing, &mWindowInfo.aaQuality);
		if (FAILED(hr)) return hr;
	}
	else {
		mWindowInfo.antialiasing = 1;
		mWindowInfo.aaQuality = 1;
	}

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = mWindowInfo.renderWidth;
	sd.BufferDesc.Height = mWindowInfo.renderHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = mWindowHandle;
	sd.SampleDesc.Count = mWindowInfo.antialiasing;
	sd.SampleDesc.Quality = mWindowInfo.aaQuality - 1;
	sd.Windowed = !mWindowInfo.fullscreen;
	

	IDXGIDevice* dxgiDevice = 0;
	hr = mDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
	if (FAILED(hr)) return hr;

	IDXGIAdapter* dxgiAdapter = 0;
	hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);
	if (FAILED(hr)) return hr;
	
	IDXGIFactory* dxgiFactory = 0;
	hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
	if (FAILED(hr)) return hr;

	hr = dxgiFactory->CreateSwapChain(mDevice, &sd, &mSwapChain);
	if (FAILED(hr)) return hr;

	dxgiDevice->Release();
	dxgiDevice = NULL;
	dxgiAdapter->Release();
	dxgiAdapter = NULL;
	dxgiFactory->Release();
	dxgiFactory = NULL;

	return hr;
}

HRESULT Core::createTargetAndDepthView()
{
	HRESULT hr = S_OK;


	ID3D11Texture2D* pBackBuffer = NULL;
	hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
		return hr;

	hr = mDevice->CreateRenderTargetView(pBackBuffer, NULL, &mRenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
		return hr;

	D3D11_TEXTURE2D_DESC dsd;
	dsd.Width = mWindowInfo.renderWidth;
	dsd.Height = mWindowInfo.renderHeight;
	dsd.MipLevels = 1;
	dsd.ArraySize = 1;
	dsd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsd.SampleDesc.Count = mWindowInfo.antialiasing;
	dsd.SampleDesc.Quality = mWindowInfo.aaQuality - 1;
	dsd.Usage = D3D11_USAGE_DEFAULT;
	dsd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dsd.CPUAccessFlags = 0;
	dsd.MiscFlags = 0;

	ID3D11Texture2D* depthStencilBuffer;

	hr = mDevice->CreateTexture2D(&dsd, 0, &depthStencilBuffer);
	if (FAILED(hr)) return hr;

	hr = mDevice->CreateDepthStencilView(depthStencilBuffer, 0, &mDepthStencilView);
	if (FAILED(hr)) return hr;

	mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

	return hr;
}

HRESULT Core::createViewPort()
{
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = mWindowInfo.renderWidth;
	vp.Height = mWindowInfo.renderHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	mDeviceContext->RSSetViewports(1, &vp);

	return S_OK;
}

HRESULT Core::createLayoutAndCompileShaders()
{
	D3D11_INPUT_ELEMENT_DESC ildesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	DEFAULT_SHADER_ID = genShader();

	HRESULT hr = compileShader(DEFAULT_SHADER_ID, "Shader.fx", "Shader.fx", ildesc, 2);
	if (FAILED(hr)) return hr;

	bindShader(DEFAULT_SHADER_ID);

	return S_OK;
}

HRESULT Core::initializeConstantBuffers()
{
	HRESULT hr = S_OK;

	hr = createConstantBuffer<SBPerApp>(D3D11_USAGE_DEFAULT, 0, 0, mDevice, mDeviceContext, &mBufferPerApp);
	if (FAILED(hr)) return hr;

	hr = createConstantBuffer<SBPerFrame>(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 1, mDevice, mDeviceContext, &mBufferPerFrame);
	if (FAILED(hr)) return hr;

	hr = createConstantBuffer<SBPerObject>(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 2, mDevice, mDeviceContext, &mBufferPerObject);
	if (FAILED(hr)) return hr;

	return hr;
}

HRESULT Core::createAndUseDefaultSampler()
{
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HRESULT hr = mDevice->CreateSamplerState(&sampDesc, &mSamplerState);
	if (FAILED(hr))
		return hr;
	mDeviceContext->PSSetSamplers(0, 1, &mSamplerState);
	return S_OK;
}
