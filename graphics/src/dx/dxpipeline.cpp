#include "dxpipeline.h"
#include <assert.h>
#include <string>

DxPipeline::DxPipeline(const PipelineCreateInfo& createInfo) :
	Pipeline(createInfo)
{
	ID3DBlob* vertexShader;
	ID3DBlob* pixelShader;

	ID3DBlob* vertexShaderError;
	ID3DBlob* pixelShaderError;

	std::wstring psSourcePathWide(createInfo.psPath.begin(), createInfo.psPath.end());
	std::wstring vsSourcePathWide(createInfo.vsPath.begin(), createInfo.vsPath.end());

	D3DCompileFromFile(vsSourcePathWide.c_str(), nullptr, nullptr, createInfo.vsEntryPoint.c_str(), "vs_5_0", 0, 0, &vertexShader, &vertexShaderError);
	D3DCompileFromFile(psSourcePathWide.c_str(), nullptr, nullptr, createInfo.psEntryPoint.c_str(), "ps_5_0", 0, 0, &pixelShader, &pixelShaderError);

	//TODO: Log error here
	assert(vertexShaderError == nullptr && pixelShaderError == nullptr);

	reflectShaders(vertexShader, pixelShader);
}

void DxPipeline::reflectShaders(ID3DBlob* vertexShader, ID3DBlob* pixelShader)
{
	ID3DBlob* shaders[] = { vertexShader, pixelShader };
	std::vector<std::string> resourcesFound;
	int nextId = 0;

	for (int shaderIndex = 0; shaderIndex < _countof(shaders); shaderIndex++)
	{
		ID3D12ShaderReflection* shaderReflection;
		D3DReflect(shaders[shaderIndex]->GetBufferPointer(), shaders[shaderIndex]->GetBufferSize(), IID_ID3D12ShaderReflection, (void**)&shaderReflection);
		D3D12_SHADER_DESC shaderDesc;
		shaderReflection->GetDesc(&shaderDesc);

		for (int resIndex = 0; resIndex < shaderDesc.BoundResources; resIndex++)
		{
			D3D12_SHADER_INPUT_BIND_DESC resBindingDesc;
			shaderReflection->GetResourceBindingDesc(resIndex, &resBindingDesc);
			if (std::count(resourcesFound.begin(), resourcesFound.end(), std::string(resBindingDesc.Name)))
				return;
			resourcesFound.push_back(std::string(resBindingDesc.Name));

			switch (resBindingDesc.Type)
			{
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_CBUFFER:
			{
				ID3D12ShaderReflectionConstantBuffer* cbuffer =
					shaderReflection->GetConstantBufferByIndex(resIndex);
				D3D12_SHADER_BUFFER_DESC cbufferDesc;
				cbuffer->GetDesc(&cbufferDesc);

				std::shared_ptr<ShaderCBufferDescription> shaderBufferDescription =
					std::make_shared<ShaderCBufferDescription>(std::string(cbufferDesc.Name),
						resBindingDesc.BindPoint, nextId, cbufferDesc.Size);

				m_bufferDescriptions[nextId] = shaderBufferDescription;

				nextId++;
			}
			}
		}
	}
}
