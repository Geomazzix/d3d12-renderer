cbuffer cbPerObject : register(b0)
{
	float4x4 ProjViewMatrix;
    float4x4 ModelMatrix;
    float4x4 NormalMatrix;
    float4 CameraPos;
};

struct VertexIn
{
	float3 Position  : POSITION;
	float3 Normal : NORMAL;
	float4 Tangent : TANGENT;
	float4 Color : COLOR;
	float2 TexCoord : TEXCOORD;
};

struct VertexOut
{
    float4 Position : SV_POSITION;
    float3 WorldPosition : POSITION;
	float3 Normal : NORMAL;
	float4 Tangent : TANGENT;
	float4 Color : COLOR;
	float2 TexCoord : TEXCOORD;
    float3 ToEye : DIR_TO_EYE;
};

VertexOut main(VertexIn IN)
{
    float4 position = mul(float4(IN.Position, 1.0f), ModelMatrix);
    float3x3 normalMat = NormalMatrix;
	
	VertexOut OUT;
    OUT.WorldPosition = position.xyz;
    OUT.Position = mul(position, ProjViewMatrix);;
    OUT.Normal = mul(IN.Normal, normalMat);
	OUT.Tangent = IN.Tangent;
	OUT.TexCoord = IN.TexCoord;
	OUT.Color = IN.Color;
    OUT.ToEye = normalize(CameraPos.xyz - position.xyz);
	
	return OUT;
}