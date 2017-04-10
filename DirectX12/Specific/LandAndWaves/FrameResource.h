#pragma once

#include"../../Common/D3DUtil.h"
#include"../../Common/MathHelper.h"
#include"../../Common/UploadBuffer.h"

//cb中的数据总是作为一个整体被提交给GPU，
//这意味着即使cb中只有一个变量改变了，
//也必须重新提交整个cb。
//所以不要把所有变量都放到一个cb中，
//而是按照变量改变的频率来组织变量，
//以尽量减少带宽消耗。

//per object constant buffer
//store constants that are associated with an object
struct ObjectConstants
{
	DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
};

//this buffer stores constant data that is fixed over a given rendering pass
//such as the eye position, the view and projection matrices, and information about the screen (render target) dimensions;
//it also includes game timing information, which is useful data to have access to in shader programs. 
struct PassConstants
{
	DirectX::XMFLOAT4X4 View = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvView = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 Proj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvProj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 ViewProj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvViewProj = MathHelper::Identity4x4();
	DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
	float cbPerObjectPad1 = 0.0f;
	DirectX::XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
	DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
	float NearZ = 0.0f;
	float FarZ = 0.0f;
	float TotalTime = 0.0f;
	float DeltaTime = 0.0f;
};

//
struct Vertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT4 Color;
};

///////////////////////////////////////////////////////////////////////////////////
// Stores the resources needed for the CPU to build the command lists for a frame.
// The contents here will vary from app to app based on the needed resources.
/////////////////////////////////////////////////////////////////////////////
//本例中加入了waveVB，它是dynamic vertex buffer
//////////////////
struct FrameResource
{
public:
	FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT waveVertCount);
	FrameResource(const FrameResource& rhs) = delete;
	FrameResource& operator=(const FrameResource& rhs) = delete;
	~FrameResource();

	//每帧需要一个allocator
	//因为GPU执行完命令之前不能reset allocator
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CmdListAlloc;

	//每帧需要与一个自己的constant buffer
	//因为GPU执行完constant buffer相关命令之前，不能对cbuffer更新
	std::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;
	std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;

	// We cannot update a dynamic vertex buffer until the GPU is done processing
	// the commands that reference it.  So each frame needs their own.
	std::unique_ptr<UploadBuffer<Vertex>> WavesVB = nullptr;

	// Fence value to mark commands up to this fence point. 
	// This lets us check if these frame resources are still in use by the GPU.
	UINT64 Fence = 0;
};