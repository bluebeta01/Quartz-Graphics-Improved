#pragma once
#include <d3d12.h>
#include <d3dcompiler.h>
#include "d3dx12.h"
#include "pipeline.h"

class DxPipeline : public Pipeline
{
public:
	DxPipeline(const PipelineCreateInfo& createInfo);

	NativeResource getNativeResource() const { return m_pipelineState; }
	ID3D12RootSignature* getRootSignature() const { return m_rootSignature; }
	ID3D12PipelineState* getPipelineState() const { return m_pipelineState; }

private:
	ID3D12RootSignature* m_rootSignature = nullptr;
	ID3D12PipelineState* m_pipelineState = nullptr;
	int m_cBufferCount = 0;
	int m_textureCount = 0;

	void reflectShaders(ID3DBlob* vertexShader, ID3DBlob* pixelShader);
};