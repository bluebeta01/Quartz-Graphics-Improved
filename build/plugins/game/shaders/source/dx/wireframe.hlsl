cbuffer matracies : register(b0)
{
	float4x4 modelMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float3x3 normalMatrix;
};

struct VOut
{
    float4 position : SV_POSITION;
	float3 pixelPos : POSITION;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD0;
};

VOut VShader(float4 position : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD, float2 lightmapcoord : TEXCOORD)
{
    VOut output;
	output.position = position;
	position.w = 1.0f;
	
    output.position = mul(position, modelMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	output.tex = texcoord;
	output.tex.y = 1 - output.tex.y;
	output.pixelPos = mul(position, modelMatrix).xyz;
	
	normal.w = 0;
	output.normal = mul(normal, modelMatrix).xyz;
    return output;
}


float4 PShader(float4 position : SV_POSITION, float3 pixelPos : POSITION, float3 normal : NORMAL, float2 tex : TEXCOORD0) : SV_TARGET
{
	return float4(1,1,1,1);
}