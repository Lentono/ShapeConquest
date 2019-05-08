
struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 texCoord : TEXCOORD;
    float4 colour : COLOUR;
};

float4 main(PS_INPUT input) : SV_Target
{
    return input.colour;

}