//per object constant buffer
//store constants that are associated with an object
cbuffer  cbPerObject : register(b0)
{
	float4x4 gWorld;
};

//this buffer stores constant data that is fixed over a given rendering pass
//such as the eye position, the view and projection matrices, and information about the screen (render target) dimensions;
//it also includes game timing information, which is useful data to have access to in shader programs.
cbuffer cbPass : register(b1)
{
	float4x4 gView;
	float4x4 gInvView;
	float4x4 gProj;
	float4x4 gInvProj;
	float4x4 gViewProj;
	float4x4 gInvViewProj;
	float3 gEyePosW;
	float cbPerObjectPad1;
	float2 gRenderTargetSize;
	float2 gInvRenderTargetSize;
	float gNearZ;
	float gFarZ;
	float gTotalTime;
	float gDeltaTime;
}