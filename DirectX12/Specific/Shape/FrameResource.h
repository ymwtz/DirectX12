#pragma once

#include"../../Common/D3DUtil.h"
#include"../../Common/MathHelper.h"
#include"../../Common/UploadBuffer.h"

struct ObjectConstants
{
	DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
};

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

struct Vertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT4 Color;
};

///////////////////////////////////////////////////////////////////////////////////
// Stores the resources needed for the CPU to build the command lists for a frame.
// The contents here will vary from app to app based on the needed resources.
/////////////////////////////////////////////////////////////////////////////
struct FrameResource
{
public:
	FrameResource(ID3D12Device *device, UINT passCount, UINT objectCount);
	FrameResource(const FrameResource &rhs) = delete;
	FrameResource &operator=(const FrameResource &rhs) = delete;
	~FrameResource();

	//每帧需要一个allocator
	//因为GPU执行完命令之前不能reset allocator
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CmdListAlloc;

	//每帧需要与一个自己的constant buffer
	//因为GPU执行完constant buffer相关命令之前，不能对cbuffer更新
	std::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;
	std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;

	// Fence value to mark commands up to this fence point. 
	// This lets us check if these frame resources are still in use by the GPU.
	UINT64 Fence = 0;
};