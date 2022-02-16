Texture2D shaderTexture : register(t0);
SamplerState SampleType;

cbuffer matracies : register(b0)
{
	float4x4 modelMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float3x3 normalMatrix;
};

cbuffer testbuffer : register(b3)
{
	float4x4 nothingfloat;
};

struct VOut
{
    float4 position : SV_POSITION;
	float3 pixelPos : POSITION;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD0;
};

VOut VShader(float4 position : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD)
{
    VOut output;
	position.w = 1.0f;
    output.position = mul(position, modelMatrix);
	output.position = mul(position, nothingfloat);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	output.tex = texcoord;
	output.pixelPos = mul(position, modelMatrix).xyz;
	
	normal.w = 0;
	output.normal = mul(normal, modelMatrix).xyz;
    return output;
}


float4 PShader(float4 position : SV_POSITION, float3 pixelPos : POSITION, float3 normal : NORMAL, float2 tex : TEXCOORD0) : SV_TARGET
{
	//return float4(normal, 1.0);
	
	float3 ambientLight = { 0.2f, 0.2f, 0.2f };
	float3 lightPos = { 0,0,0 };
	normal = normalize(normal);
	float3 lightDir = normalize(lightPos - pixelPos);
	float diff = max(dot(normal, lightDir), 0.0);
	float3 lightColor = {1,1,1};
	float3 diffuse = diff * lightColor;
	
	float3 result = (ambientLight + diffuse);
	
	float4 textureColor;
    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, tex);
	
    return float4(result, 1.0f) * textureColor;
	
	
	//return float4(tex, 1.0, 1.0);
	
	
	return textureColor;
    //return textureColor * float4((ambientLight + diffuse) * c, 1.0f);
}