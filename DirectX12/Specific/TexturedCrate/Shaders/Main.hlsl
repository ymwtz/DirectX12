//Ĭ�Ϲ�Դ
#ifndef NUM_DIR_LIGHTS
    #define NUM_DIR_LIGHTS 1
#endif

#ifndef NUM_POINT_LIGHTS
    #define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
    #define NUM_SPOT_LIGHTS 0
#endif

//
#include "LightUtil.hlsl"

//Constant data
cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;
};

cbuffer cbMaterial : register(b1)
{
	float4 gDiffuseAlbedo;
	float3 gFresnelR0;
	float gRoughness;
	float4x4 gMatTransform;
};

cbuffer cbPass : register(b2)
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

struct VertexIn
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
};


VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut) 0.0f;

	//�任����������
	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
	vout.PosW = posW.xyz;

	//����û�н��зǾ�������
	//����Ӧ��ʹ��world matrix��������ת�þ���inverse-transpose��
	vout.NormalW = mul(vin.NormalL, (float3x3) gWorld);

	//�任��homogeneous clip space
	vout.PosH = mul(posW, gViewProj);

	return vout;
}

float4 PS(VertexOut pin):SV_Target
{
	//��ֵ���ɵ�normal�����Ƿǹ�һ����
	//���ԶԵ�ķ��������й�һ��
	pin.NormalW = normalize(pin.NormalW);

	//Vector from point being lit to eye
	float3 toEyeW = normalize(gEyePosW - pin.PosW);

	//Indirect Lighting(������)
	float4 ambient = gAmbientLight * gDiffuseAlbedo;

	//Direct Lighting
	const float shiniess = 1.0f - gRoughness;
	Material mat = { gDiffuseAlbedo, gFresnelR0, shiniess };
	float3 shadowFactor = 1.0f;
	float4 directLight = ComputeLighting(gLights, mat, pin.PosW, pin.NormalW, toEyeW, shadowFactor);

	float4 litColor = ambient + directLight;

	//
	litColor.a = gDiffuseAlbedo.a;

	return litColor;
}