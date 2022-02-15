#pragma once
#include <d3d12.h>
#include <d3dcompiler.h>
#include "d3dx12.h"
#include "pipeline.h"

class DxPipeline : public Pipeline
{
public:
	DxPipeline(const PipelineCreateInfo& createInfo);

	ID3D12RootSignature* getRootSignature() const { return m_rootSignature; }
	ID3D12PipelineState* getPipelineState() const { return m_pipelineState; }

private:
	ID3D12RootSignature* m_rootSignature = nullptr;
	ID3D12PipelineState* m_pipelineState = nullptr;

	void reflectShaders(ID3DBlob* vertexShader, ID3DBlob* pixelShader);
};