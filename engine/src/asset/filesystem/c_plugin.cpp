#include "pch.h"
#include "asset/filesystem/c_plugin.h"
#include "asset/filesystem/c_filestream.h"

namespace Quartz
{
Plugin::Plugin()
{

}

bool Plugin::containsFile(const std::string& path)
{
	std::string filePath = "plugins/";
	filePath.append(m_name);
	filePath.append("/");
	filePath.append(path);
	FileStream stream;
	return stream.loadFile(filePath.c_str(), true);
}
}