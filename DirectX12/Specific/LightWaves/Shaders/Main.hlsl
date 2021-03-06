//默认光源
#ifndef NUM_DIR_LIGHTS
    #define NUM_DIR_LIGHTS 3
#endif

#ifndef NUM_POINT_LIGHTS
    #define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
    #define NUM_SPOT_LIGHTS 0
#endif

//
#include "LightingUtil.hlsl"

//
Texture2D gDiffuseMap : register(t0);

//6种静态sampler
//SamplerState gsamPointWrap : register(s0);
//SamplerState gsamPointClamp : register(s1);
//SamplerState gsamLinearWrap : register(s2);
//SamplerState gsamLinearClamp : register(s3);
//SamplerState gsamAnisotropicWrap : register(s4);
//SamplerState gsamAnisotropicClamp : register(s5);

SamplerState gsamLinear  : register(s0);

//Constant data that varies per frame
cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;
    float4x4 gTexTransform;
};

//
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
	float4 gAmbientLight;

    // Indices [0, NUM_DIR_LIGHTS) are directional lights;
    // indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
    // indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
    // are spot lights for a maximum of MaxLights per object.
	Light gLights[MaxLights];
}

//Constant data that varies per material
cbuffer cbMaterial : register(b2)
{
    float4 gDiffuseAlbedo;
    float3 gFresnelR0;
    float gRoughness;
    float4x4 gMatTransform;
};

struct VertexIn
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
    float2 TexC : TEXCOORD;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
    float2 TexC : TEXCOORD;
};


VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut) 0.0f;

	//变换到世界坐标
	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
	vout.PosW = posW.xyz;

	//假设没有进行非均匀缩放
	//否则应该使用world matrix的逆矩阵的转置矩阵（inverse-transpose）
	vout.NormalW = mul(vin.NormalL, (float3x3) gWorld);

	//变换到homogeneous clip space
	vout.PosH = mul(posW, gViewProj);

    //
    float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform);
    vout.TexC = mul(texC, gMatTransform).xy;
	return vout;
}

float4 PS(VertexOut pin):SV_Target
{
	float4 diffuseAlbedo = gDiffuseMap.Sample(gsamLinear, pin.TexC) * gDiffuseAlbedo;

	//插值生成的normal可能是非归一化的
	//所以对点的法向量进行归一化
	pin.NormalW = normalize(pin.NormalW);

	//Vector from point being lit to eye
	float3 toEyeW = normalize(gEyePosW - pin.PosW);

	//Indirect Lighting(环境光)
	float4 ambient = gAmbientLight * diffuseAlbedo;

	//Direct Lighting
	const float shiniess = 1.0f - gRoughness;
	Material mat = { diffuseAlbedo, gFresnelR0, shiniess };
	float3 shadowFactor = 1.0f;
	float4 directLight = ComputeLighting(gLights, mat, pin.PosW, pin.NormalW, toEyeW, shadowFactor);

	float4 litColor = ambient + directLight;

	//
	litColor.a = diffuseAlbedo.a;

	return litColor;
}