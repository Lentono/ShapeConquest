Texture2D myTexture : register(t0);
SamplerState sampleState;

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 texCoord : TEXCOORD;
};

float4 main(PS_INPUT input) : SV_Target
{ 
	return myTexture.Sample(sampleState, input.texCoord);//float4(input.texCoord, 0,1);
}