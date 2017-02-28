#include "D3DApp.h"
#include "D3DUtil.h"

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

////////////////
bool D3DApp::InitDirect3D()
{
#if defined(DEBUG)||defined(_DEBUG)
	{
		ComPtr<ID3D12Debug> debugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();
	}
#endif

	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&mdxgiFactory)));

	//尝试创建硬件设备
	HRESULT hardwareResult = D3D12CreateDevice(
		nullptr,
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&md3dDevice));

	//若失败，则回退到WARP设备
	if (FAILED(hardwareResult))
	{
		ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(mdxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(
			pWarpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&md3dDevice)));
	}

	return false;
}
