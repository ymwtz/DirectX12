cbuffer cbPerObject:register(b0) {
	float4x4 gWorldViewProj;
};

struct VertexIn {
	float3 PosL:POSITION;
	float4 Color:COLOR;
};

struct VertexOut {
	float4 PosH:SV_POSITION;
	float4 Color:COLOR;
};

//vertex shader
VertexOut VS(VertexIn vin) {
	VertexOut vout;

	//变换坐标
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

	//输出给pixel shader的颜色
	vout.Color = vin.Color;

	return vout;
}

//pixel shader
float4 PS(VertexOut pin) : SV_Target{
	return pin.Color;
}