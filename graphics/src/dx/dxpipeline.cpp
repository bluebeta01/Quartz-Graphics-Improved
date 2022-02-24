#include "dxpipeline.h"
#include <assert.h>
#include <string>
#include <algorithm>
#include <iostream>

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
	//assert(vertexShaderError == nullptr && pixelShaderError == nullptr);
	if (vertexShaderError)
		std::cout << "Vertex shader error: " << (char*)vertexShaderError->GetBufferPointer() << "\n";
	if(pixelShaderError)
		std::cout << "Pixel shader error: " << (char*)pixelShaderError->GetBufferPointer() << "\n";

	reflectShaders(vertexShader, pixelShader);

	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
	featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
	if (FAILED(((ID3D12Device*)m_device->getNativeResource())->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	
	CD3DX12_DESCRIPTOR_RANGE1 cbvRange = {};
	cbvRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, m_cBufferCount, 0);

	CD3DX12_DESCRIPTOR_RANGE1 srvRange = {};
	srvRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, m_textureCount, 0);

	CD3DX12_ROOT_PARAMETER1 cbvRootParams = {};
	cbvRootParams.InitAsDescriptorTable(1, &cbvRange, D3D12_SHADER_VISIBILITY_VERTEX);

	CD3DX12_ROOT_PARAMETER1 srvRootParams = {};
	srvRootParams.InitAsDescriptorTable(1, &srvRange, D3D12_SHADER_VISIBILITY_PIXEL);

	std::vector<CD3DX12_ROOT_PARAMETER1> params;
	if (m_cBufferCount != 0)
		params.push_back(cbvRootParams);
	m_cbvRootIndex = params.size() - 1;
	if (m_textureCount != 0)
		params.push_back(srvRootParams);
	m_srvRootIndex = params.size() - 1;


	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_STATIC_SAMPLER_DESC sampler = {};
	sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.MipLODBias = 0;
	sampler.MaxAnisotropy = 0;
	sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	sampler.MinLOD = 0.0f;
	sampler.MaxLOD = D3D12_FLOAT32_MAX;
	sampler.ShaderRegister = 0;
	sampler.RegisterSpace = 0;
	sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	sampler.Filter = D3D12_FILTER::D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_1(params.size(), params.data(), 1, &sampler, rootSignatureFlags);

	ID3DBlob* signature;
	ID3DBlob* error;
	D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error);
	((ID3D12Device*)m_device->getNativeResource())->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));


	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		//{ "SV_InstanceID", 0, DXGI_FORMAT_R32_UINT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1}
	};
	
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = m_rootSignature;
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader);
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader);
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
	if(!createInfo.backfaceCulling)
		psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	if(!createInfo.depthTest)
		psoDesc.DepthStencilState.DepthEnable = false;
	if(createInfo.conservativeRaster)
		psoDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE::D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON;
	if (createInfo.wireframe)
		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	if(createInfo.useUint32)
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_UINT;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // a default depth stencil state
	((ID3D12Device*)m_device->getNativeResource())->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));
	
	vertexShader->Release();
	pixelShader->Release();
}

void DxPipeline::reflectShaders(ID3DBlob* vertexShader, ID3DBlob* pixelShader)
{
	ID3DBlob* shaders[] = { vertexShader, pixelShader };
	std::vector<std::string> resourcesFound;

	std::vector<std::shared_ptr<ShaderBindableDescription>> cBuffers;
	std::vector<std::shared_ptr<ShaderBindableDescription>> textures;

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

				std::shared_ptr<ShaderBindableDescription> bufferBindableDescription = std::make_shared<ShaderBindableDescription>();
				bufferBindableDescription->name = cbufferDesc.Name;
				bufferBindableDescription->slot = resBindingDesc.BindPoint;
				bufferBindableDescription->tableIndex = 0;
				bufferBindableDescription->type = ShaderVariableType::CBUFFER;

				cBuffers.push_back(bufferBindableDescription);
				m_cBufferCount++;
				break;
			}

			case D3D_SHADER_INPUT_TYPE::D3D_SIT_TEXTURE:
			{
				std::shared_ptr<ShaderBindableDescription> textureBindableDescription = std::make_shared<ShaderBindableDescription>();
				textureBindableDescription->name = resBindingDesc.Name;
				textureBindableDescription->slot = resBindingDesc.BindPoint;
				textureBindableDescription->tableIndex = 0;
				textureBindableDescription->type = ShaderVariableType::TEXTURE;

				textures.push_back(textureBindableDescription);
				m_textureCount++;
				break;
			}
			}
		}
	}

	std::sort(cBuffers.begin(), cBuffers.end(),
		[](std::shared_ptr<ShaderBindableDescription> b1, std::shared_ptr<ShaderBindableDescription> b2)
		{
			return b1->slot < b2->slot;
		}
	);

	std::sort(textures.begin(), textures.end(),
		[](std::shared_ptr<ShaderBindableDescription> b1, std::shared_ptr<ShaderBindableDescription> b2)
		{
			return b1->slot < b2->slot;
		}
	);

	for (int i = 0; i < cBuffers.size(); i++)
		cBuffers[i]->tableIndex = i;

	for (int i = 0; i < textures.size(); i++)
		textures[i]->tableIndex = i;

	m_bindables.insert(m_bindables.end(), cBuffers.begin(), cBuffers.end());
	m_bindables.insert(m_bindables.end(), textures.begin(), textures.end());

	m_cBufferCount = cBuffers.size();
	m_textureCount = textures.size();
}
