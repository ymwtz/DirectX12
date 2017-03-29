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
	//������ת��Ϊ�ѿ�������
	float x = mRadius*sinf(mPhi)*cosf(mTheta);
	float z = mRadius*sinf(mPhi)*cosf(mTheta);
	float y = mRadius*cosf(mPhi);

	//Build the view martix
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, view);

	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX worldViewProj = world*view*proj;
	
	//�����µ�world view proj����update the costant buffer
	ObjectConstants objConstants;
	XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(worldViewProj));
	mObjectCB->CopyData(0, objConstants);
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
	//specify what kind of primitive the vertices define
	//point, line lists or triangle lists
	mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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

//////////
//�ı��������
//////////
void BoxApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0) {
		//ÿ�ƶ�һ�����أ���Ӧ�Ƕȱ仯0.25��
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		//���½Ƕ�
		mTheta += dx;
		mPhi += dy;

		//����������0-����
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
	}
	else if ((btnState&MK_RBUTTON) != 0) {
		//ÿ�ƶ�һ�����أ���Ӧ������0.005����λ
		float dx = 0.005f*static_cast<float>(x - mLastMousePos.x);
		float dy = 0.005f*static_cast<float>(y - mLastMousePos.y);

		//Update the camera radius
		mRadius += dx - dy;

		//��radius������3-15��
		mRadius = MathHelper::Clamp(mRadius, 3.0f, 15.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

//
void BoxApp::BuildDescriptorHeaps()
{
}

/////////////
//create constant buffer
///////////
void BoxApp::BuildConstantBuffers()
{
	mObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(md3dDevice.Get(), 1, true);

	UINT elementByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

}

//
void BoxApp::BuildRootSignature()
{
}

//
void BoxApp::BuildShadersAndInputLayout()
{
	//description of vertex structure
	mInputLayout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
}

////////////
//Input Assembler stage
//����vertex buffer��index buffer
///////////
void BoxApp::BuildBoxGeometry()
{
	//�洢���������8���㣬ÿ����һ����ɫ
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

	//�洢�����������������ɼ�����
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

	//vertex buffer�ĳߴ�
	const UINT64 vbByteSize = vertices.size() * sizeof(Vertex);
	//index buffer�ĳߴ�
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	mBoxGeo = std::make_unique<MeshGeometry>();
	mBoxGeo->Name = "boxGeo";

	//����vertex buffer
	mBoxGeo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(
		md3dDevice.Get(),
		mCommandList.Get(),
		vertices.data(),
		vbByteSize,
		mBoxGeo->VertexBufferUploader
	);
	//����index buffer
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

