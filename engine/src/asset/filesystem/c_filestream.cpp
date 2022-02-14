#include "pch.h"
#include "asset/filesystem/c_filestream.h"

namespace Quartz
{
FileStream::FileStream() : m_file(nullptr)
{

}

FileStream::~FileStream()
{
	close();
}

bool FileStream::loadFile(const std::string& path, bool isText)
{
	if (m_file)
		fclose(m_file);
	fopen_s(&m_file, path.c_str(), isText ? "r" : "rb");
	if (!m_file)
		QZERROR("Failed to load file \"{0}\"", path);
	return m_file;
}

void FileStream::close()
{
	if (m_file)
		fclose(m_file);
	m_file = nullptr;
}

bool FileStream::fileExists(const std::string& path)
{
	FILE* file;
	fopen_s(&file, path.c_str(), "r");
	if (!file)
		return false;
	else
	{
		fclose(file);
		return true;
	}
}

std::string FileStream::readAllText()
{
	if (!m_file)
		return "";
	std::string result;
	char c;
	while (!feof(m_file))
	{
		fread_s(&c, sizeof(char), sizeof(char), 1, m_file);
		result += c;
	}
	result.erase(result.end()-1);
	return result;
}

void FileStream::readBytes(unsigned int numberOfBytes, void* destination)
{
	if (!m_file)
		return;

	fread_s(destination, numberOfBytes, sizeof(unsigned char), numberOfBytes, m_file);
}

std::string FileStream::readString()
{
	if (!m_file)
		return "";

	int stringLength = 0;
	fread_s(&stringLength, sizeof(stringLength), sizeof(unsigned char), sizeof(stringLength), m_file);
	char* string = new char[stringLength];
	fread_s(string, sizeof(unsigned char) * stringLength, sizeof(unsigned char), stringLength, m_file);
	std::string result(string);
	delete[] string;
	return result;
}

void FileStream::readInt(int* destination)
{
	if (!m_file)
		return;

	fread_s(destination, sizeof(int), sizeof(unsigned char), sizeof(int), m_file);
}

}