#include "Initial.h"

/////////
InitDirect3DApp::InitDirect3DApp(HINSTANCE hInstance)
	: D3DApp(hInstance)
{
}

///////////
InitDirect3DApp::~InitDirect3DApp()
{
}

///////////////
bool InitDirect3DApp::Initialize()
{
	if (!D3DApp::Initialize())
		return false;
	return true;
}

////////////
void InitDirect3DApp::OnResize()
{
	D3DApp::OnResize();
}

void InitDirect3DApp::Update(const GameTimer & gt)
{

}

///////////////
//Draw��ÿһ֡�б�����
//���ҵ��õĵط��Ƿ�����Ⱦ����������ǰ֡����buckbuffer�ϵ�ʱ��
//����ɻ��ƣ�����IDCGISwapChain::Present����back buffer�������Ļ��
///////////////
void InitDirect3DApp::Draw(const GameTimer & gt)
{
	//Reuse	the	memory associated with command recording.
	//We can only reset when the associated command lists have finished execution on the GPU.
	ThrowIfFailed(mDirectCmdListAlloc->Reset());

	//
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	//
	mCommandList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		)
	);

	//
	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	//
	mCommandList->ClearRenderTargetView(
		CurrentBackBufferView(),
		Colors::LightSteelBlue,
		0,
		nullptr
	);
	mCommandList->ClearDepthStencilView(
		DepthStencilView(),
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		1.0f,
		0,
		0,
		nullptr
	);

	//
	mCommandList->OMSetRenderTargets(
		1,
		&CurrentBackBufferView(),
		true,
		&DepthStencilView()
	);

	//
	mCommandList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT
		)
	);

	//
	ThrowIfFailed(mCommandList->Close());

	//
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(
		_countof(cmdsLists),
		cmdsLists
	);

	//
	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

	//
	FlushCommandQueue();
}