cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
	float4 colour;
};

struct GeometryShaderInput
{
	float4 position : SV_POSITION;
	float2 particleSize : TEXCOORD0;
};

struct PixelShaderInput
{
	float4 positionH : SV_POSITION;
	float3 positionW : POSITION;
	float2 texcoord : TEXCOORD0;
	float4 colour : COLOR0;
};

[maxvertexcount(6)]
void main(point GeometryShaderInput input[1], inout TriangleStream<PixelShaderInput> outputStream)
{
	PixelShaderInput output;

	float4 position = input[0].position;

	output.positionW = mul(position, model);
	position = mul(position, model);
	position = mul(position, view);

	static float3 positions[4] =
	{
		float3(-0.5,0.5,0),
		float3(0.5,0.5,0),
		float3(-0.5,-0.5,0),
		float3(0.5,-0.5,0)
	};

	/*Triange 1*/

	//Vertex1
	float quadSize = input[0].particleSize.x;
	output.positionH = position + float4(quadSize*positions[0], 0.0);
	output.positionH = mul(output.positionH, projection);
	output.colour = colour;
	output.texcoord = float2(0.0, 1.0f);

	outputStream.Append(output);

	//Vertex2
	output.positionH = position + float4(quadSize*positions[2], 0.0);
	output.positionH = mul(output.positionH, projection);
	output.colour = colour;
	output.texcoord = float2(0.0, 0.0);

	outputStream.Append(output);

	//Vertex3
	output.positionH = position + float4(quadSize*positions[1], 0.0);
	output.positionH = mul(output.positionH, projection);
	output.colour = colour;
	output.texcoord = float2(1.0, 1.0f);

	outputStream.Append(output);

	outputStream.RestartStrip();

	/*Triangle 2*/

	//Vertex1
	output.positionH = position + float4(quadSize*positions[1], 0.0);
	output.positionH = mul(output.positionH, projection);
	output.colour = colour;
	output.texcoord = float2(1.0, 1.0);

	outputStream.Append(output);

	//Vertex2
	output.positionH = position + float4(quadSize*positions[2], 0.0);
	output.positionH = mul(output.positionH, projection);
	output.colour = colour;
	output.texcoord = float2(0.0, 0.0);

	outputStream.Append(output);

	//Vertex3
	output.positionH = position + float4(quadSize*positions[3], 0.0);
	output.positionH = mul(output.positionH, projection);
	output.colour = colour;
	output.texcoord = float2(1.0, 0.0);

	outputStream.Append(output);
}