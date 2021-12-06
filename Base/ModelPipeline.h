#pragma once
#include "IGraphicsPipeline.h"

class ModelPipeline : public IGraphicsPipeline
{
private:
	ModelPipeline();
	~ModelPipeline();
public:

	const PipeClass::PipelineSet &GetPipeLine();

	static ModelPipeline *GetInstance();
private:
	PipeClass::PipelineSet pipelineSet;
};