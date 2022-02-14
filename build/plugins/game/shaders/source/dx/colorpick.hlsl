cbuffer matracies : register(b0)
{
	float4x4 modelMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float4 color;
};

struct VOut
{
    float4 position : SV_POSITION;
	float3 pixelPos : POSITION;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD0;
	float4 color : COLOR;
};

VOut VShader(float4 position : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD)
{
    VOut output;
	
	//position.z += 5;
	
	position.w = 1.0f;
    output.position = mul(position, modelMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	output.normal = normal.xyz;
	output.tex = texcoord;
	output.color = color;
	output.pixelPos = mul(modelMatrix, position).xyz;

    return output;
}


float4 PShader(float4 position : SV_POSITION, float3 pixelPos : POSITION, float3 normal : NORMAL, float2 tex : TEXCOORD0, float4 color : COLOR) : SV_TARGET
{
	return color;
}