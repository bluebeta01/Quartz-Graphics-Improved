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
	int getCBVRootIndex() const { return m_cbvRootIndex; }
	int getSRVRootIndex() const { return m_srvRootIndex; }

private:
	ID3D12RootSignature* m_rootSignature = nullptr;
	ID3D12PipelineState* m_pipelineState = nullptr;
	int m_cbvRootIndex = 0;
	int m_srvRootIndex = 0;

	void reflectShaders(ID3DBlob* vertexShader, ID3DBlob* pixelShader);
};