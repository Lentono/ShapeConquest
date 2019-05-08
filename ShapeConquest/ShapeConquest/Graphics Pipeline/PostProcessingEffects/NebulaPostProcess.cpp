#include "NebulaPostProcess.h"
#include "../../Managers/ResourceManager.h"


NebulaPostProcess::NebulaPostProcess(const std::shared_ptr<RenderTarget>& renderTarget, const std::shared_ptr<Shader>& shader)
	: PostProcessingEffect(renderTarget, shader)
{
	_currentPosition = { RandDecimal() ,RandDecimal() };
	//_targetPosition = { RandDecimal() ,RandDecimal() };
	_currentCol1 = { RandDecimal() ,RandDecimal(),RandDecimal() };
	_targetCol1 = { RandDecimal() ,RandDecimal(),RandDecimal() };
	_currentCol2 = { RandDecimal() ,RandDecimal(),RandDecimal() };
	_targetCol2 = { RandDecimal() ,RandDecimal(),RandDecimal() };
	col1LerpVal = RandDecimal() * changeTime;
	col2LerpVal = RandDecimal() * changeTime;
	/*posLerpVal = RandDecimal() * changeTime;*/
}


NebulaPostProcess::~NebulaPostProcess()
{
}

float NebulaPostProcess::RandDecimal()
{
	return (rand() % 100) / 100.0;
}

void NebulaPostProcess::Run(std::shared_ptr<Texture> prevOutput, std::shared_ptr<Graphics> graphics, float dt)
{
	_time += dt;
	_renderTarget->Prep(graphics);
	_shader->Load(graphics);

	col1LerpVal += dt;
	col2LerpVal += dt;
	/*posLerpVal += dt;*/

	if(col1LerpVal > changeTime)
	{
		_targetCol1 = { RandDecimal() ,RandDecimal(),RandDecimal() };
		col1LerpVal = 0;
	}
	if (col2LerpVal > changeTime)
	{
		_targetCol2 = { RandDecimal() ,RandDecimal(),RandDecimal() };
		col2LerpVal = 0;
	}
	/*if (posLerpVal > changeTime)
	{
		_targetPosition = { RandDecimal() ,RandDecimal()};
		posLerpVal = 0;
	}*/

	_currentCol1 = glm::mix(_currentCol1, _targetCol1, col1LerpVal / changeTime);
	_currentCol2 = glm::mix(_currentCol2, _targetCol2, col2LerpVal / changeTime);
	/*_currentPosition = glm::mix(_currentPosition, _targetPosition, posLerpVal / changeTime);*/

	NebularCB cb;
	cb.time = _time;
	cb.timeMul = 1;
	cb.nebulaColour= _currentCol1;
	cb.backgroundColour =  _currentCol2;
	cb.dimensions = glm::vec2(graphics->Width(), graphics->Height());
	cb.vanishingPoint = (_currentPosition - glm::vec2(0.5,0.5)) * 0.05f;
	_shader->UpdateBuffer(ShaderTypes::Pixel, &cb, graphics);

	auto view = prevOutput->GetView();
	graphics->DeviceContext3D()->PSSetShaderResources(0, 1, &view);
	ID3D11SamplerState * pp_samplers = graphics->DefaultSampleState();
	graphics->DeviceContext3D()->PSSetSamplers(0, 1, &pp_samplers);
	ResourceManager::GetGeometry("PPEdefault")->Render(graphics);
}
