#include <iostream>
#include <tinyxml2.h>
#include <string>
#include <vector>

struct Qmf
{
	std::string modelName;
	std::string materialName;
	int numberOfVerts;
	int floatsPerVert;
	std::vector<float> data;
};

std::vector<std::string> splitString(std::string string)
{
	std::vector<std::string> strings;
	std::string currentString = "";
	for (int i = 0; i < string.size(); i++)
	{
		if (string[i] == ' ')
		{
			strings.push_back(currentString);
			currentString = "";
			continue;
		}
		currentString.push_back(string[i]);
	}
	strings.push_back(currentString);
	return strings;
}

std::vector<float> stringArrayToFloatArray(std::vector<std::string> strings)
{
	std::vector<float> floats;
	for (int i = 0; i < strings.size(); i++)
	{
		floats.push_back(std::stof(strings[i].c_str()));
	}
	return floats;
}

std::vector<int> stringArrayToIntArray(std::vector<std::string> strings)
{
	std::vector<int> ints;
	for (std::string string : strings)
	{
		ints.push_back(std::stoi(string));
	}
	return ints;
}

void logMessage(const char* message)
{
	std::cout << message << "\n";
}

void writeQmf(const char* filepath, const Qmf& qmf)
{
	const char* magic = "QUARTZMODELFILE";
	int version = 1;
	int modelNameLength = qmf.modelName.size() + 1;
	int materialNameLength = qmf.materialName.size() + 1;
	int vertCount = qmf.numberOfVerts;
	int floatsPerVert = qmf.floatsPerVert;

	FILE* file;
	fopen_s(&file, filepath, "wb");
	fwrite(magic, sizeof(unsigned char), 16, file);
	fwrite(&version, sizeof(unsigned char), sizeof(version), file);
	fwrite(&modelNameLength, sizeof(unsigned char), sizeof(modelNameLength), file);
	fwrite(qmf.modelName.c_str(), sizeof(unsigned char), modelNameLength, file);
	fwrite(&materialNameLength, sizeof(unsigned char), sizeof(materialNameLength), file);
	fwrite(qmf.materialName.c_str(), sizeof(unsigned char), materialNameLength, file);
	fwrite(&vertCount, sizeof(unsigned char), sizeof(vertCount), file);
	fwrite(&floatsPerVert, sizeof(unsigned char), sizeof(floatsPerVert), file);

	fwrite(qmf.data.data(), sizeof(float), qmf.data.size(), file);

	fclose(file);
}

void convert(const char* filepath)
{
	std::vector<float> positions;
	std::vector<float> normals;
	std::vector<float> uvs;
	std::vector<float> uvs2;
	std::vector<int> tris;
	int triCount = 0;

	tinyxml2::XMLDocument doc;
	doc.LoadFile(filepath);

	std::string geoId = doc.FirstChildElement("COLLADA")->FirstChildElement("library_geometries")->FirstChildElement("geometry")->FindAttribute("id")->Value();
	
	tinyxml2::XMLElement* sourceElement = doc.FirstChildElement("COLLADA")->FirstChildElement("library_geometries")->FirstChildElement("geometry")->
		FirstChildElement("mesh")->FirstChildElement("source");
	
	while (sourceElement != nullptr)
	{
		if (!std::strcmp(sourceElement->Name(), "source"))
		{
			std::string sourceId = sourceElement->FindAttribute("id")->Value();

			if (sourceId == geoId + "-positions")
			{
				std::string value = sourceElement->FirstChildElement("float_array")->GetText();
				positions = stringArrayToFloatArray(splitString(value));
			}

			if (sourceId == geoId + "-normals")
			{
				std::string value = sourceElement->FirstChildElement("float_array")->GetText();
				normals = stringArrayToFloatArray(splitString(value));
			}

			if (sourceId == geoId + "-map-0")
			{
				std::string value = sourceElement->FirstChildElement("float_array")->GetText();
				uvs = stringArrayToFloatArray(splitString(value));
			}

			if (sourceId == geoId + "-map-1")
			{
				std::string value = sourceElement->FirstChildElement("float_array")->GetText();
				uvs2 = stringArrayToFloatArray(splitString(value));
			}
		}

		if (!std::strcmp(sourceElement->Name(), "triangles"))
		{
			triCount = sourceElement->FindAttribute("count")->IntValue();
			std::string value = sourceElement->FirstChildElement("p")->GetText();
			tris = stringArrayToIntArray(splitString(value));
		}
		sourceElement = sourceElement->NextSiblingElement();
	}

	Qmf qmf = {};
	qmf.materialName = doc.FirstChildElement("COLLADA")->FirstChildElement("library_materials")->FirstChildElement("material")->FindAttribute("name")->Value();
	for (int i = 0; i < qmf.materialName.size(); i++)
	{
		if (qmf.materialName[i] == '-')
			qmf.materialName[i] = char('/');
	}
	qmf.materialName += ".qmat";
	qmf.modelName = doc.FirstChildElement("COLLADA")->FirstChildElement("library_geometries")->FirstChildElement("geometry")->FindAttribute("name")->Value();
	qmf.floatsPerVert = 10;
	qmf.numberOfVerts = triCount * 3;

	for (int i = 0; i < tris.size(); i += 3)
	{
		int positionIndex = tris[i];
		int normalIndex = tris[i + 1];
		int uvIndex = tris[i + 2];

		qmf.data.push_back(positions[(positionIndex * 3)]);
		qmf.data.push_back(positions[(positionIndex * 3) + 1]);
		qmf.data.push_back(positions[(positionIndex * 3) + 2]);

		qmf.data.push_back(normals[(normalIndex * 3)]);
		qmf.data.push_back(normals[(normalIndex * 3) + 1]);
		qmf.data.push_back(normals[(normalIndex * 3) + 2]);

		qmf.data.push_back(uvs[(uvIndex * 2)]);
		qmf.data.push_back(uvs[(uvIndex * 2) + 1]);

		qmf.data.push_back(uvs2[(uvIndex * 2)]);
		qmf.data.push_back(uvs2[(uvIndex * 2) + 1]);
	}

	std::string path = filepath;
	path.resize(strlen(filepath) - 4);
	path += ".qmf";
	writeQmf(path.c_str(), qmf);
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		logMessage("No file provided");
		return 1;
	}

	convert(argv[1]);
	//std::cin.get();

	return 0;
}