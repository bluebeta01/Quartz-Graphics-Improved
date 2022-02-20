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

VOut VShader(float4 position : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD, float2 lightmapcoord : TEXCOORD1)
{
    VOut output;
	output.position = position;
	position.w = 1.0f;
	output.position.w = 1.0f;
	output.position.z = 0;
	output.position.xy = lightmapcoord;
	output.position.x *= 2;
	output.position.x -= 1;
	output.position.y *= 2;
	output.position.y -= 1;
	
	output.pixelPos = mul(position, modelMatrix).xyz;
	float4 tempnormal = float4(normal.xyz, 0);
	output.normal = mul(tempnormal, modelMatrix).xyz;
	output.tex = texcoord;
	
    return output;
}


float4 PShader(float4 position : SV_POSITION, float3 pixelPos : POSITION, float3 normal : NORMAL, float2 tex : TEXCOORD0) : SV_TARGET
{
	float3 ambientLight = { 0.2f, 0.2f, 0.2f };
	float3 lightPos = { 0,0,0 };
	normal = normalize(normal);
	float3 lightDir = normalize(lightPos - pixelPos);
	float diff = max(dot(normal, lightDir), 0.0);
	float3 lightColor = {1,1,1};
	float3 diffuse = diff * lightColor;
	
	float3 result = (ambientLight + diffuse);
	
	return float4(result, 1.0f);
}