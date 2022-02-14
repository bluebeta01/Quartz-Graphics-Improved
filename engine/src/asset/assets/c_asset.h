#pragma once
#include <string>
#include <memory>

namespace Quartz
{
class Asset
{
public:
	enum class Type
	{
		TEXT,
		TEXTURE2D,
		MATERIAL,
		MODEL,
		SHADER
	};
	enum class Status
	{
		LOAD_REQUESTED,
		READY_FOR_GPU_UPLOAD,
		GPU_UPLOAD_REQUESTED,
		LOADED,
		NOT_LOADED
	};
	Type m_type;
	std::string m_name;
	std::string m_absolutePath;
	Status m_loadStatus;
	bool m_unloadedDependencies;
	Asset(Type type, const std::string& name, const std::string& absolutePath) : 
		m_type(type), m_name(name), m_absolutePath(absolutePath), m_loadStatus(Status::LOAD_REQUESTED),
		m_unloadedDependencies(false) {};
	virtual ~Asset() {};
	virtual void load(std::shared_ptr<Asset> asset) = 0;
	virtual bool childrenLoaded() { return m_loadStatus == Status::LOADED; };
	virtual void loadDependencies() {};
};
}