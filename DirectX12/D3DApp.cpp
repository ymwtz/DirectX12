#include "D3DApp.h"

///////////
using namespace std;
using Microsoft::WRL::ComPtr;
using namespace DirectX;
///////////

//////////////
D3DApp::D3DApp(HINSTANCE hInstance)
	:mhAppInst(hInstance)
{
	//只能创建一个
	assert(hInstance);
	mApp = this;
}

////////////
D3DApp::~D3DApp()
{
	if (md3dDevice != nullptr)
		FlushCommandQueue();
}

///////////
D3DApp * D3DApp::GetApp()
{
	return mApp;
}

/////////////////
HINSTANCE D3DApp::AppInst() const
{
	return mhAppInst;
}

/////////////////
HWND D3DApp::MainWnd() const
{
	return mhMainWnd;
}

/////////////
int D3DApp::Run()
{
	return 0;
}

//////////////
bool D3DApp::Initialize()
{
	if (!InitMainWindow())
		return false;
	if (!InitDirect3D())
		return false;

	OnResize();

	return true;
}
