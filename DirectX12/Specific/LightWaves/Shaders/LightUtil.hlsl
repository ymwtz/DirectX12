//����Դ��
#define MaxLights 16

//hlsl��4ά���������ͬһ�������ܲ�
//���Ա���˳����Ҫ��
struct Light {
	float3 Strength;
	float FalloffStart;	//ֻ��point/spot light��Ҫ
	float3 Direction;
	float FalloffEnd;
	float3 Position;
	float SpotPower;
};