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
float3 SchlickApprox(float3 normal, float3 f0, float3 lightDir);
float CalculateLightAttenuation(in float3 lightPosition, in float3 position, float intensity, float range);

float4 main(VertexOut IN) : SV_Target
{
    float3 toEye = IN.ToEye;
    float3 normal = IN.Normal;
    
    float3 ambientLight = float3(0.3, 0.3, 0.3);	
    float3 color = float3(0.0, 0.0, 0.0);
    
    for (int i = 0; i < DirectionalLightCount; i++)
    {
        float3 lightRadiance = DirectionalLights[i].Color * DirectionalLights[i].Intensity;
        color += BlinnPhongLighting(normal, toEye, -DirectionalLights[i].Direction, FresnelR0, RoughnessFactor, lightRadiance, AlbedoDiffuse.rgb);
    }
    
    for (int i = 0; i < PointLightCount; i++)
    {
        float3 lightRadiance = PointLights[i].Color * CalculateLightAttenuation(PointLights[i].Position, IN.WorldPosition.xyz, PointLights[i].Intensity, PointLights[i].Radius);
        float3 lightDir = normalize(PointLights[i].Position - IN.WorldPosition.xyz);
        color += BlinnPhongLighting(normal, toEye, lightDir, FresnelR0, RoughnessFactor, lightRadiance, AlbedoDiffuse.rgb);
    }
    
    float3 ambient = AlbedoDiffuse.rgb * ambientLight;
    
    color = clamp(color, float3(0, 0, 0), float3(1, 1, 1));
    return float4(color, 1.0);
}

float3 BlinnPhongLighting(in float3 normal, in float3 toEye, in float3 lightDir, in float3 r0, in float roughness, in float3 lightRadiance, in float3 albedoColor)    
{
    float3 specularColor = float3(0.5, 0.5, 0.5);
    float3 halfwayVector = normalize(toEye + lightDir);
    
    float roughnessFactor = pow(max(dot(normal, halfwayVector), 0.0), roughness);
    float3 fresnelFactor = SchlickApprox(normal, r0, lightDir);

    float ndotl = max(dot(lightDir, normal), 0.0);
    return (albedoColor + roughnessFactor * fresnelFactor * specularColor) * lightRadiance * ndotl;
}

float3 SchlickApprox(float3 normal, float3 f0, float3 lightDir)
{
    return float3(f0 + (1.0 - f0) * (1 - cos(dot(lightDir, normal))));
}

float CalculateLightAttenuation(in float3 lightPosition, in float3 position, float intensity, float range)
{
    float distance = length(lightPosition - position);

    float E = intensity;
    float DD = range * range;
    float Q = 1;
    float rr = distance * distance;

    return intensity * (DD / (DD + Q * rr));
}