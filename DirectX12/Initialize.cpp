#include "Initialize.h"

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

void InitDirect3DApp::Draw(const GameTimer & gt)
{
}
