#include "ShapesApp.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

//包含3个Frame Resource的序列
const int gNumFrameResources = 3;

//
//创建包含3个Frame Resource的序列
//
void ShapesApp::BuildFrameResources()
{
	for (int i = 0;i < gNumFrameResources;i++) {
		mFrameResources.push_back(
			std::make_unique<FrameResource>(
				md3dDevice.Get(),
				1,
				(UINT)mAllRitems.size()
			)
		);
	}
}

//
void ShapesApp::Update(const GameTimer & gt)
{
	OnKeyboardInput(gt);
	UpdateCamera(gt);

	//对于CPU frame n
	// Cycle through the circular frame resource array.
	mCurrFrameResourceIndex = (mCurrFrameResourceIndex + 1) % gNumFrameResources;
	mCurrFrameResource = mFrameResources[mCurrFrameResourceIndex].get();

	//如果GPU还没执行完当前frame resource的命令，
	//则等待直到GPU completed commands up to this fence point
	if (mCurrFrameResource->Fence != 0 && mFence->GetCompletedValue() < mCurrFrameResource->Fence) {
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(mFence->SetEventOnCompletion(mCurrFrameResource->Fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	//更新mFrameResource中的resource

}

//
void ShapesApp::Draw(const GameTimer & gt)
{
	//Build and submit command lists for this frame.

	//Advance the fence value to mark commands up to this fence point.
	mCurrFrameResource->Fence = ++mCurrentFence;

	// Add an instruction to the command queue to set a new fence point. 
	// Because we are on the GPU timeline, the new fence point won't be set, 
	// until the GPU finishes processing all the commands prior to this Signal().
	mCommandQueue->Signal(mFence.Get(), mCurrentFence);

	// Note that GPU could still be working on commands from previous frames,
	//but that is okay, because we are not touching any frame  resources associated with those frames.
}

//
void ShapesApp::UpdateObjectCBs(const GameTimer & gt)
{
	auto currObjectCB = mCurrFrameResource->ObjectCB.get();
	for (auto& e : mAllRitems) {
		//只有constants改变，才update cbuffer data
		//This needs to be tracked per frame resource.
		if (e->NumFramesDirty > 0) {
			XMMATRIX world = XMLoadFloat4x4(&e->World);

			ObjectConstants objConstants;
			XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));

			currObjectCB->CopyData(e->ObjectCBIndex, objConstants);

			//Next FrameResource need to be updated too.
			e->NumFramesDirty--;
		}
	}
}

//
void ShapesApp::UpdateMainPassCB(const GameTimer & gt)
{
}
