cbuffer ConstantBuffer : register(b0)
{
    float4x4 model;
	float4x4 view;
	float4x4 projection;
    float4 colour;
}

struct VS_INPUT
{
	float3 pos : POSITION;
	float2 texCoord : TEXCOORD;
	float3 normal : NORMAL;
};

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 texCoord : TEXCOORD;
    float4 colour : COLOUR;
};

PS_INPUT main(VS_INPUT input)
{
	PS_INPUT output;
	output.pos = float4(input.pos, 1);
    output.pos = mul(output.pos, model);
	output.pos = mul(output.pos, view);
	output.pos = mul(output.pos, projection);
	output.texCoord = input.texCoord;
    output.colour = colour;
	return output;
}
