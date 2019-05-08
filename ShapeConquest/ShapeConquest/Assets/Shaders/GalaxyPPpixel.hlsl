Texture2D myTexture : register(t0);
SamplerState sampleState;

//Taken from : https://www.shadertoy.com/view/XslGRr    By IQ

cbuffer nebularBuffer : register(b0)
{
	float timeMul;
	float3 colour;
	float time;
	float3 backgroundColour;
	float2 dimensions;
	float2 vanishingPoint;
}

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 texCoord : TEXCOORD;
};

static int steps = 2;
static float3 sunDir = float3(1, 0, 0);

float hash(float n)
{
	return frac(sin(n)*43758.5453);
}

float noise(float3 x)
{
	float3 p = floor(x);
	float3 f = frac(x);

	f = f * f*(3.0 - 2.0*f);
	float n = p.x + p.y*57.0 + 113.0*p.z;

	return lerp(lerp(lerp(hash(n + 0.0), hash(n + 1.0), f.x),
		lerp(hash(n + 57.0), hash(n + 58.0), f.x), f.y),
		lerp(lerp(hash(n + 113.0), hash(n + 114.0), f.x),
			lerp(hash(n + 170.0), hash(n + 171.0), f.x), f.y), f.z);
}

float4 Intergrate(float4 sum, float dif, float density, float dist)
{
	float3 lin = (colour* 0.8) *1.4 + float3(1.0, 0.6, 0.3)*dif;
	float4 col = float4(lerp(float3(1.0, 0.95, 0.8), float3(0.25, 0.3, 0.35), density), density);

	col.xyz *= lin;
	col.xyz = lerp(col.xyz, (backgroundColour * 0.8), 1.0 - exp(-0.003*dist*dist));

	col.a *= 0.4;

	col.rgb *= col.a;
	return  sum + col * (1.0 - sum.a);

}

float map5(float3 p)
{
	float3 q = p - normalize(float3(vanishingPoint, 1.0)) * time * timeMul;
	float f;
	f = 0.50000*noise(q); 
	q = q * 2.02;
	f += 0.25000*noise(q); 
	q = q * 2.03;
	f += 0.12500*noise(q); 
	q = q * 2.01;
	f += 0.06250*noise(q); 
	q = q * 2.02;
	f += 0.03125*noise(q);
	return clamp(1.5 - p.y - 2.0 + 1.75*f, 0.0, 1.0);
}

float4 RayMarch(float3 origin, float3 dir)
{
	float4 sum = 0;

	float dist = 1;

	for (int i = 0; i < steps; i++)
	{
		float3 pos = origin + dir * dist;
		if (sum.a > 0.99)
		{
			break;
		}
		float density = map5(pos);
		if (density > 0.01)
		{
			float dif = clamp((density - map5(pos + 0.3*sunDir)) / 0.6, 0, 1);
			sum = Intergrate(sum, dif, density, dist);
		}
		dist += max(0.05, 0.02 * dist);
	}
	sum = clamp(sum, 0, 1);
	float3 col = (backgroundColour * 0.8);
	col = col * (1.0 - sum.w) + sum.xyz;
	return float4(col, 1);
}

float4 main(PS_INPUT input) : SV_Target
{
	float4 output = myTexture.Sample(sampleState, input.texCoord);
	if (output.a > 0.9)
	{
		return output;
	}

	//ray march cloud

	//float2 p = (-dimensions.xy + 2 * input.texCoord) / dimensions.y;
	float2 p = (input.pos + float2(1,1)) / 2;
	float3 dir = normalize(float3(input.texCoord.xy * float2(2, 2), 1.5));

	////return float4(noise(float3(p.xy * 1, 1)), 0, 0, 1);
	//int numAOne = 0;
	//float2 pixelSize = float2(1 / dimensions.x, 1 / dimensions.y);
	//float4 posCol = float4(0, 0, 0, 0);
	//float count = 0;
	//int range = 3;
	//for (int i = -range; i < range + 1; i++)
	//{
	//	for (int j = -range; j < range + 1; j++)
	//	{
	//		float4 temp = myTexture.Sample(sampleState, input.texCoord + (pixelSize * float2(i, j)));
	//		if (temp.a > 0.5)
	//		{
	//			numAOne++;
	//			posCol += temp;
	//		}
	//		count++;
	//	}
	//}
	//float lVal = 0;
	//float4 rayMarched = float4(RayMarch(float3(0, 0, 0), dir + float3(-0.0, -0.5, 0)).xyz, 0);
	//if (numAOne > 1)
	//{
	//	lVal = (float)numAOne / count;
	//	return lerp(rayMarched, posCol / numAOne, lVal);
	//}
	//else
	//{
	//	return rayMarched;
	//}
	float4 rayMarched = float4(RayMarch(float3(0, 0, 0), dir + float3(-0.0, -0.5, 0)).xyz, 0);

	return lerp(rayMarched, output, output.a);
}