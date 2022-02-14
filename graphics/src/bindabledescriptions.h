#pragma once
#include <string>
#include <vector>

enum class ShaderVariableType
{
	FLOAT4X4, FLOAT3X3, TEXTURE2D, SAMPLER
};

struct ShaderVariableDescription
{
	std::string name;
	ShaderVariableType type;
};

struct ShaderBindableDescription
{
	std::string name;
	int slot;
	ShaderVariableType type;
};

class ShaderCBufferDescription
{
public:
	ShaderCBufferDescription(const std::string& name, int slot);

	void addVariableDescription(const ShaderVariableDescription& description);

	const std::string& getName() const { return m_name; }
	const std::vector<ShaderVariableDescription>& getVariables() const { return m_variables; }

private:
	std::string m_name;
	int m_slot;
	std::vector<ShaderVariableDescription> m_variables;
};