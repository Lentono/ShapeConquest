//cbuffer Projection : register(b0)
//{
//	matrix projection;
//};

struct GeomInput
{
	float4 pos : SV_Position;
};

GeomInput main(uint id : SV_VertexID)
{
	GeomInput output;
	output.pos = float4(0,0, 0, 1);
	return output;
}