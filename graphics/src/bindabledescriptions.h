#pragma once
#include <string>

enum class ShaderVariableType
{
	NONE, CBUFFER, TEXTURE
};

struct ShaderBindableDescription
{
	std::string name;
	int slot;
	int tableIndex;
	ShaderVariableType type;
};