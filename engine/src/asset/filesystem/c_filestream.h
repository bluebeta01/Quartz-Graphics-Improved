#pragma once
#include <string>
#include <iostream>

namespace Quartz
{
class FileStream
{
public:
	std::string m_filePath;
	FILE* m_file;
	FileStream();
	~FileStream();
	bool loadFile(const std::string& path, bool isText);
	void close();
	static bool fileExists(const std::string& path);
	std::string readAllText();
	void readBytes(unsigned int numberOfBytes, void* destination);
	std::string readString();
	void readInt(int* destination);
};
}