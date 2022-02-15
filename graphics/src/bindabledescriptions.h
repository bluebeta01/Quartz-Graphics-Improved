#pragma once
#include <string>
#include <vector>

enum class ShaderVariableType
{
	NONE, FLOAT4X4, FLOAT3X3, TEXTURE2D, SAMPLER
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
	ShaderCBufferDescription(const std::string& name, int slot, int id, int size);

	void addVariableDescription(const ShaderVariableDescription& description);

	const std::string& getName() const { return m_name; }
	const std::vector<ShaderVariableDescription>& getVariables() const { return m_variables; }
	const int getId() const { return m_id; }
	const int getSize() const { return m_size; }

private:
	std::string m_name;
	int m_slot = 0;
	std::vector<ShaderVariableDescription> m_variables;
	int m_id = 0;
	int m_size = 0;
};