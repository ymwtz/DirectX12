//最大光源数
#define MaxLights 16

//hlsl按4维向量打包，同一个量不能拆开
//所以变量顺序影响空间大小
struct Light
{
	float3 Strength;
	float FalloffStart; // point/spot light only
	float3 Direction; // directional/spot light only
	float FalloffEnd; // point/spot light only
	float3 Position; // point light only
	float SpotPower; // spot light only
};

struct Material
{
	float4 DiffuseAlbedo;
	float3 FresnelR0;
	float Shininess;
};

//计算光基于点与光源距离的衰减
float CalcAttenuation(float d, float falloffStart, float falloffEnd)
{
	//线性衰减
	return saturate((falloffEnd - d) / (falloffEnd - falloffStart));
}

//Approximates the percentage of light reflected off a surface 
//with normal n based on the angle between the light vector L and surface normal n due to the Fresnel effect. 
//返回反射率
float3 SchlickFresnel(float3 R0, float3 normal, float3 lightVec)
{
	float cosIncidentAngle = saturate(dot(normal, lightVec));

	float f0 = 1.0f - cosIncidentAngle;
	float3 reflectPercent = R0 + (1.0f - R0) * (f0 * f0 * f0 * f0 * f0);

	return reflectPercent;
}

//返回反射光强（漫反射加镜面反射）
float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye, Material mat)
{
	//由shininess得出m的值，shininess由roughness得出
	const float m = mat.Shininess * 256.0f;
	float3 halfVec = normalize(toEye + lightVec);

	float roughnessFactor = (m + 8.0f) * pow(max(dot(halfVec, normal), 0.0f), m) / 8.0f;
	float3 fresnelFactor = SchlickFresnel(mat.FresnelR0, halfVec, lightVec);

	float3 specAlbedo = fresnelFactor * roughnessFactor;

    // Our spec formula goes outside [0,1] range, but we are 
    // doing LDR rendering.  So scale it down a bit.
	specAlbedo = specAlbedo / (specAlbedo + 1.0f);

	return (mat.DiffuseAlbedo.rgb + specAlbedo) * lightStrength;
}

//directional light的光照方程
float3 ComputeDirectionalLight(Light L, Material mat, float3 normal, float3 toEye)
{
	//light vector方向与光线照射方向相反
	float3 lightVec = -L.Direction;

    // Scale light down by Lambert's cosine law.
	float ndotl = max(dot(lightVec, normal), 0.0f);
	float3 lightStrength = L.Strength * ndotl;

	return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

//point light的光照方程
float3 ComputrPointLight(Light L, Material mat, float3 pos, float3 normal, float toEye)
{
	//从表面指向光源的向量
	float3 lightVec = L.Position - pos;

	//表面到光源的距离
	float d = length(lightVec);

	//距离测试
	if (d > L.FalloffEnd)
		return 0.0f;

	//Normalize the light vector
	lightVec /= d;

	//Scale light down by Lambert's cosine law
	float ndotl = max(dot(lightVec, normal), 0.0f);
	float3 lightStrength = L.Strength * ndotl;

	//
	float att = CalcAttenuation(d, L.FalloffStart, L.FalloffEnd);
	lightStrength *= att;

	return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

//spot light的光照方程
float3 ComputeSpotLight(Light L, Material mat, float3 pos, float3 normal, float3 toEye)
{
    // The vector from the surface to the light.
	float3 lightVec = L.Position - pos;

    // The distance from surface to light.
	float d = length(lightVec);

    // Range test.
	if (d > L.FalloffEnd)
		return 0.0f;

    // Normalize the light vector.
	lightVec /= d;

    // Scale light down by Lambert's cosine law.
	float ndotl = max(dot(lightVec, normal), 0.0f);
	float3 lightStrength = L.Strength * ndotl;

    // Attenuate light by distance.
	float att = CalcAttenuation(d, L.FalloffStart, L.FalloffEnd);
	lightStrength *= att;

    // Scale by spotlight
	float spotFactor = pow(max(dot(-lightVec, L.Direction), 0.0f), L.SpotPower);
	lightStrength *= spotFactor;

	return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

//
float4 ComputeLighting(Light gLights[MaxLights], Material mat,
                       float3 pos, float3 normal, float3 toEye,
                       float3 shadowFactor)
{
	float3 result = 0.0f;

	int i = 0;

#if (NUM_DIR_LIGHTS > 0)
    for(i = 0; i < NUM_DIR_LIGHTS; ++i)
    {
        result += shadowFactor[i] * ComputeDirectionalLight(gLights[i], mat, normal, toEye);
    }
#endif

#if (NUM_POINT_LIGHTS > 0)
    for(i = NUM_DIR_LIGHTS; i < NUM_DIR_LIGHTS+NUM_POINT_LIGHTS; ++i)
    {
        result += ComputePointLight(gLights[i], mat, pos, normal, toEye);
    }
#endif

#if (NUM_SPOT_LIGHTS > 0)
    for(i = NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS + NUM_SPOT_LIGHTS; ++i)
    {
        result += ComputeSpotLight(gLights[i], mat, pos, normal, toEye);
    }
#endif 

	return float4(result, 0.0f);
}