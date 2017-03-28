#include "Box.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace std;

//
BoxApp::BoxApp(HINSTANCE hInstance)
{
}

//
BoxApp::~BoxApp()
{
}

//
bool BoxApp::Initialize()
{
	return false;
}

//
void BoxApp::OnResize()
{
}

//
void BoxApp::Update(const GameTimer & gt)
{
}

//
void BoxApp::Draw(const GameTimer & gt)
{
	ThrowIfFailed(mDirectCmdListAlloc->Reset());

	ThrowIfFailed(mCommandList->Reset(
		mDirectCmdListAlloc.Get(),
		mPSO.Get()
	));

	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	//bind vertex buffer to an input slot of pipeline
	//to feed the vertices to the input assembler stage of pipeline
	mCommandList->IASetVertexBuffers(0, 1, &mBoxGeo->VertexBufferView());
	//bind index buffer to input assembler stage
	mCommandList->IASetIndexBuffer(&mBoxGeo->IndexBufferView());

	//draw the vertices
	mCommandList->DrawIndexedInstanced(
		mBoxGeo->DrawArgs["box"].IndexCount,
		1, 0, 0, 0
	);
}

//
void BoxApp::OnMouseDown(WPARAM btnState, int x, int y)
{
}

//
void BoxApp::OnMouseUp(WPARAM btnState, int x, int y)
{
}

//
void BoxApp::OnMouseMove(WPARAM btnState, int x, int y)
{
}

//
void BoxApp::BuildDescriptorHeaps()
{
}

//
void BoxApp::BuildConstantBuffers()
{
}

//
void BoxApp::BuildRootSignature()
{
}

//
void BoxApp::BuildShadersAndInputLayout()
{
}

////////////
//Input Assembler stage
//创建vertex buffer和index buffer
///////////
void BoxApp::BuildBoxGeometry()
{
	//存储了正方体的8个点，每个点一种颜色
	array<Vertex, 8> vertices = {
		Vertex({ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White) }),
		Vertex({ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Black) }),
		Vertex({ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Red) }),
		Vertex({ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green) }),
		Vertex({ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Blue) }),
		Vertex({ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow) }),
		Vertex({ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Cyan) }),
		Vertex({ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Magenta) })
	};

	//存储顶点的索引，用来组成几何体
	std::array<std::uint16_t, 36> indices = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	//vertex buffer的尺寸
	const UINT64 vbByteSize = vertices.size() * sizeof(Vertex);
	//index buffer的尺寸
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	mBoxGeo = std::make_unique<MeshGeometry>();
	mBoxGeo->Name = "boxGeo";

	//创建vertex buffer
	mBoxGeo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(
		md3dDevice.Get(),
		mCommandList.Get(),
		vertices.data(),
		vbByteSize,
		mBoxGeo->VertexBufferUploader
	);
	//创建index buffer
	mBoxGeo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(
		md3dDevice.Get(),
		mCommandList.Get(),
		indices.data(),
		ibByteSize,
		mBoxGeo->IndexBufferUploader
	);
}

//
void BoxApp::BuildPSO()
{
}

