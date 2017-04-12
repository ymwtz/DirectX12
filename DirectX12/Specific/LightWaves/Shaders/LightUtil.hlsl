//����Դ��
#define MaxLights 16

//hlsl��4ά���������ͬһ�������ܲ�
//���Ա���˳��Ӱ��ռ��С
struct Light {
	float3 Strength;
	float FalloffStart;	//ֻ��point/spot light��Ҫ
	float3 Direction;
	float FalloffEnd;
	float3 Position;
	float SpotPower;
};

struct Material {
	float4 DiffuseAlbedo;
	float3 FresnelR0;
	float Shininess;
};

//�������ڵ����Դ�����˥��
float CalAttenuation(float d, float falloffStart, float falloffEnd) {
	//����˥��
	return saturate((falloffEnd - d) / (falloffEnd - falloffStart));
}

//Approximates the percentage of light reflected off a surface 
//with normal n based on the angle between the light vector L and surface normal n due to the Fresnel effect. 
//���ط�����
float3 SchlickFresnel(float3 R0, float3 normal, float3 lightVec) {
	float cosIncidentAngle = saturate(dot(normal, lightVec));
	float f0 = 1.0f - cosIncidentAngle;
	float3 reflectPercent = R0 + (1.0f - R0)*(f0*f0*f0*f0);

	return reflectPercent;
}

//���ط����ǿ��������Ӿ��淴�䣩
float3 BlinnPhong(float3 lightStrngth, float3 lightVec, float3 normal, float3 toEye, Material mat) {
	//��shininess�ó�m��ֵ��shininess��roughness�ó�
	const float m = mat.Shininess*256.0f;
	float3 halfVec = normalize(toEye + lightVec);

	float roughnessFactor = (m + 8.0f)*pow(max(dot(halfVec, normal), 0.0f), m) / 8.0f;
	float3 fresnelFactor = SchlickFresnel(mat.FresnelR0, halfVec, lightVec);

	// Our spec formula goes outside [0,1] range, but we are 
	// doing LDR rendering.  So scale it down a bit.
	float3 specAlbedo = fresnelFactor*roughnessFactor;
	specAlbedo = specAlbedo / (specAlbedo + 1.0f);

	return (mat.DiffuseAlbedo.rgb + specAlbedo)*lightStrength;
}

//