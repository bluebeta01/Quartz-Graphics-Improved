Texture2D diffuse : register(t0);
Texture2D lightmap : register(t1);

SamplerState SampleType;

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
	float2 lightmapcoords : TEXCOORD1;
};

VOut VShader(float4 position : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD, float2 lightmapcoord : TEXCOORD1)
{
    VOut output;
	output.position = position;
	position.w = 1.0f;

	
    output.position = mul(position, modelMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	output.tex = texcoord;
	output.tex.y = 1 - output.tex.y;
	output.lightmapcoords = lightmapcoord;
	output.lightmapcoords.y = 1 - output.lightmapcoords.y;
	output.pixelPos = mul(position, modelMatrix).xyz;
	
	normal.w = 0;
	output.normal = mul(normal, modelMatrix).xyz;
    return output;
}


float4 PShader(float4 position : SV_POSITION, float3 pixelPos : POSITION, float3 normal : NORMAL, float2 tex : TEXCOORD0, float2 lightmapcoord : TEXCOORD1) : SV_TARGET
{
	//return float4(normal, 1.0);
	
	float3 ambientLight = { 0.2f, 0.2f, 0.2f };
	float3 lightPos = { 0,0,0 };
	normal = normalize(normal);
	float3 lightDir = normalize(lightPos - pixelPos);
	float diff = max(dot(normal, lightDir), 0.0);
	float3 lightColor = {1,1,1};
	float3 diffuseColor = diff * lightColor;
	
	float3 result = (ambientLight + diffuseColor);
	
	float4 textureColor;
    textureColor = diffuse.Sample(SampleType, tex);
	float4 lightval = lightmap.Sample(SampleType, lightmapcoord);
	
	
	return textureColor * lightval;
}