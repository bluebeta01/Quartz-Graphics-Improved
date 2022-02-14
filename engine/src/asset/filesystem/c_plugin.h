#pragma once
#include <string>

namespace Quartz
{
class Plugin
{
public:
	std::string m_friendlyName;
	std::string m_name;
	std::string m_version;
	std::string m_desc;
	Plugin();
	bool containsFile(const std::string& path);
};
}