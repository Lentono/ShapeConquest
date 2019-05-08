struct GeomInput
{
	float4 pos : SV_Position;
};
struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 texCoord : TEXCOORD;
};


[maxvertexcount(6)]
void main(point GeomInput input[1], inout TriangleStream<PS_INPUT> OutputStream)
{
	PS_INPUT output;

	output.pos = float4(-1, -1, 0, 1);
	output.texCoord = float2(0,1);
	OutputStream.Append(output);

	output.pos = float4(1, -1, 0, 1);
	output.texCoord = float2(1, 1);
	OutputStream.Append(output);

	output.pos = float4(-1, 1, 0, 1);
	output.texCoord = float2(0, 0);
	OutputStream.Append(output);
	
	OutputStream.RestartStrip();

	output.pos = float4(-1, 1, 0, 1);
	output.texCoord = float2(0, 0);
	OutputStream.Append(output);

	output.pos = float4(1, -1, 0, 1);
	output.texCoord = float2(1, 1);
	OutputStream.Append(output);


	output.pos = float4(1, 1, 0, 1);
	output.texCoord = float2(1, 0);
	OutputStream.Append(output);
}