struct PixelShaderInput
{
	float4 positionH : SV_POSITION;
	float3 positionW : POSITION;
	float2 texcoord : TEXCOORD0;
	float4 colour : COLOR0;
};

float4 main(PixelShaderInput input) : SV_Target
{
	float range = 0.5f * 0.5f - dot(input.texcoord - 0.5f, input.texcoord - 0.5f);

	float shade = 2.0f / (1.0f + exp(12 * range));

	float4 colour = saturate(float4((1 - shade) * input.colour.xyz, 1.0f));

	if ((colour.x + colour.y + colour.z / 3) < 0.5f)
	{
		discard;
	}

	return colour;
}