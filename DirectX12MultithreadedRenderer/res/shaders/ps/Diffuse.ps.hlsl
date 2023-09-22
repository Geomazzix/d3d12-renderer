#define MAX_DIRECTIONAL_LIGHTS 4
#define MAX_POINT_LIGHTS 8

struct VertexOut
{
	float4 Position  : SV_POSITION;
    float3 WorldPosition : POSITION;
	float3 Normal : NORMAL;
	float4 Tangent : TANGENT;
	float4 Color : COLOR;
	float2 TexCoord : TEXCOORD;
    float3 ToEye : DIR_TO_EYE;
};

cbuffer Material : register(b1)
{
    float4 AlbedoDiffuse;
    float3 FresnelR0;
    uint RoughnessFactor;
};

struct DirectionalLight
{
    float3 Direction;
    float Intensity;
    float3 Color;
    float Padding;
};

struct PointLight
{
    float3 Position;
    float Intensity;
    float3 Color;
    float Radius;
};

cbuffer LightConstants : register(b2)
{
    uint DirectionalLightCount;
    uint PointLightCount;
};

StructuredBuffer<DirectionalLight> DirectionalLights : register(t0);
StructuredBuffer<PointLight> PointLights : register(t1);

float3 BlinnPhongLighting(in float3 normal, in float3 toEye, in float3 lightDir, in float3 r0, in float roughness, in float3 lightRadiance, in float3 albedoColor);
float CalculateLightAttenuation(in float3 lightPosition, in float3 position, in float intensity, in float range);
float3 SchlickApprox(in float3 normal, in float3 f0, in float3 lightDir);

float4 main(VertexOut IN) : SV_Target
{
    float3 toEye = normalize(IN.ToEye);
    float3 normal = normalize(IN.Normal);
    
    float3 color = float3(0, 0, 0); // = AlbedoDiffuse.rgb * float3(0.1, 0.1, 0.1); //Setting ambient color.
    
    [unroll(MAX_DIRECTIONAL_LIGHTS)]
    for (int i = 0; i < DirectionalLightCount; i++)
    {
        float3 lightRadiance = DirectionalLights[i].Color * DirectionalLights[i].Intensity;
        color += BlinnPhongLighting(normal, toEye, normalize(-DirectionalLights[i].Direction), FresnelR0, RoughnessFactor, lightRadiance, AlbedoDiffuse.rgb);
    }
    
    [unroll(MAX_POINT_LIGHTS)]
    for (int j = 0; j < PointLightCount; j++)
    {
        float3 lightRadiance = PointLights[j].Color * CalculateLightAttenuation(PointLights[j].Position, IN.WorldPosition.xyz, PointLights[j].Intensity, PointLights[j].Radius);
        float3 lightDir = normalize(PointLights[j].Position - IN.WorldPosition.xyz);
        color += BlinnPhongLighting(normal, toEye, lightDir, FresnelR0, RoughnessFactor, lightRadiance, AlbedoDiffuse.rgb);
    }
    
    float3 srgb = pow(color, float3(1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f));
    return float4(srgb, 1.0f);
}

float3 BlinnPhongLighting(in float3 normal, in float3 toEye, in float3 lightDir, in float3 r0, in float roughness, in float3 lightRadiance, in float3 albedoColor)    
{
    float3 diffuse = albedoColor * max(dot(lightDir, normal), 0.0);
    float3 specular = float3(0.5f, 0.5f, 0.5f) * pow(max(dot(normalize(lightDir + toEye), normal), 0.0), 1024);
    
    return lightRadiance * (diffuse + specular);
}

float CalculateLightAttenuation(in float3 lightPosition, in float3 position, in float intensity, in float range)
{
    float distance = length(lightPosition - position);

    float E = intensity;
    float DD = range * range;
    float Q = 1;
    float rr = distance * distance;

    return intensity * (DD / (DD + Q * rr));
}

float3 SchlickApprox(in float3 normal, in float3 f0, in float3 lightDir)
{
    return float3(f0 + (1.0 - f0) * (1 - cos(dot(lightDir, normal))));
}