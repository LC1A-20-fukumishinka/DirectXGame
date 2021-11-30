#pragma once
#include "IGraphicsPipeline.h"

class GraphicsPipeline : public IGraphicsPipeline
{
private:
	GraphicsPipeline();
	~GraphicsPipeline();
public:

	const PipeClass::PipelineSet &GetPipeLine();

	static GraphicsPipeline *GetInstance();
private:
	PipeClass::PipelineSet pipelineSet;
};