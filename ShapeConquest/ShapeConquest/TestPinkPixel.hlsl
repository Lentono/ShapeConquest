struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 texCoord : TEXCOORD;
};

float4 main(PS_INPUT input) : SV_Target
{
	return float4(float3(1, 0.412, 0.706) * input.texCoord.x,1);
}