Texture2D myTexture : register(t0);
SamplerState sampleState;

cbuffer blurFactors : register(b0)
{
	float2 dimensions;
	int blurDepth;
	float padding;
}

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 texCoord : TEXCOORD;
};

float4 main(PS_INPUT input) : SV_Target
{
	float4 output = myTexture.Sample(sampleState, input.texCoord);
	if (output.a > 0.5)
	{
		return output;
	}
	float2 pixelSize = float2(1 / dimensions.x, 1 / dimensions.y);
	int range = 5;
	int count = 0;
	for (int i = -range; i < range + 1; i++)
	{
		for (int j = -range; j < range + 1; j++)
		{
			output += myTexture.Sample(sampleState, input.texCoord + (pixelSize * float2(i, j)));
			count++;
		}
	}
	output /= count / 2;
	return output;// lerp(float4(0.21, 0.1, 0.38, 1), output, output.a);
}