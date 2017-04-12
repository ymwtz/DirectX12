//最大光源数
#define MaxLights 16

//hlsl按4维向量打包，同一个量不能拆开
//所以变量顺序有要求
struct Light {
	float3 Strength;
	float FalloffStart;	//只有point/spot light需要
	float3 Direction;
	float FalloffEnd;
	float3 Position;
	float SpotPower;
};