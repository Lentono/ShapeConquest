#define TOTAL_EMITTERS 4

struct EmitterParameters
{
	float particleSystemHeight;
	float particleSpeed;
	float particleSpread;
	float particleSystemShape;
	float particleShape;
	float particleSize;
};

static EmitterParameters emitters[TOTAL_EMITTERS] =
{
	//StraightEmitter
	{1.5f, 0.3f, 0.5f, 1.0f, 0.6f, 0.05f},
	//ConeEmitter
	{1.5f, 0.3f, 0.5f, 1.0f, 0.6f, 0.03f},
	//ExplosionEmitter
	{1.5f, 1.0f, 0.8f, 1.5f, 0.2f, 0.03f},
	//SpiralEmitter
	{1.5f, 1.0f, 0.8f, 1.5f, 0.2f, 0.03f}
};

cbuffer ParticleSystemConstantBuffer : register(b0)
{
	float3 direction;
	float totalTime;
	int emitterType;
	float lifeTime;
	float2 padding;
}

struct VertexInput
{
	float3 position : POSITION;
	float3 instancePosition : INSTANCEPOSITION;
};

struct GeometryInput
{
	float4 position : SV_POSITION;
	float2 particleSize : TEXCOORD0;
};

float4 StraightEmitter(float3 position)
{
	float t = frac(position.z + emitters[emitterType].particleSpeed * totalTime);
	float s = pow(t, emitters[emitterType].particleSystemShape);

	float4 particlePosition;

	particlePosition.x = emitters[emitterType].particleSpread * s * cos(62.0f * position.z);
	particlePosition.z = emitters[emitterType].particleSpread * s * sin(163.0f * position.z);
	particlePosition.y = emitters[emitterType].particleSystemHeight * t;
	particlePosition.w = 1.0f;

	//if (position.z + emitters[emitterType].particleSpeed * totalTime > totalTime)
	//{
	//	particlePosition.y += 100.0f;
	//}

	//particlePosition.xyz *= direction;

	return particlePosition;
}

float4 ConeEmitter(float3 position)
{
	float t = frac(position.z + emitters[emitterType].particleSpeed * totalTime);
	float s = pow(t, emitters[emitterType].particleSystemShape);

	float4 particlePosition;

	particlePosition.x = emitters[emitterType].particleSpread * s * cos(62.0f * position.z);
	particlePosition.z = emitters[emitterType].particleSpread * s * sin(163.0f * position.z);
	particlePosition.y = emitters[emitterType].particleSystemHeight * totalTime * t;
	particlePosition.w = 1.0f;

	if (position.z + emitters[emitterType].particleSpeed * totalTime > totalTime * 1.5f)
	{
	particlePosition.y += 100.0f;
	}

	return particlePosition;
}

float4 ExplosiveEmitter(float3 position)
{
	float t = frac(position.z + emitters[emitterType].particleSpeed * totalTime);
	float s = pow(t, emitters[emitterType].particleSystemShape);

	float4 particlePosition;

	particlePosition.x = emitters[emitterType].particleSpread * s * cos(62.0f * position.z);
	particlePosition.z = emitters[emitterType].particleSpread * s * sin(163.0f * position.z);
	particlePosition.y = emitters[emitterType].particleSpread * s * sin(62.0f * position.z);
	particlePosition.w = 1.0f;

	if (position.z + emitters[emitterType].particleSpeed * totalTime > totalTime * 2.5f)
	{
		particlePosition.y += 100.0f;
	}

	return particlePosition;
}

float4 SpiralEmitter(float3 position)
{
	float t = frac(position.z + emitters[emitterType].particleSpeed * totalTime);
	float s = pow(t, emitters[emitterType].particleSystemShape);

	float4 particlePosition;

	particlePosition.x = emitters[emitterType].particleSpread * s * cos(163.0f * position.z);
	particlePosition.z = emitters[emitterType].particleSpread * s * sin(163.0f * position.z);
	particlePosition.y = emitters[emitterType].particleSpread * s * sin(163.0f * position.z);
	particlePosition.w = 1.0f;

	//if (position.z + emitters[emitterType].particleSpeed * totalTime > totalTime * 2.5f)
	//{
	//	particlePosition.y += 100.0f;
	//}

	return particlePosition;
}

GeometryInput main(VertexInput input)
{
	GeometryInput output;

	input.position += input.instancePosition;

	//Receive emitter type and run function
	if (emitterType == 0)
	{
		output.position = StraightEmitter(input.position);
	}
	else if (emitterType == 1)
	{
		output.position = ConeEmitter(input.position);
	}
	else if (emitterType == 2)
	{
		output.position = ExplosiveEmitter(input.position);
	}
	else if (emitterType == 3)
	{
		output.position = SpiralEmitter(input.position);
	}

	output.particleSize = float2(emitters[emitterType].particleSize, 0.0f);

	return output;
}