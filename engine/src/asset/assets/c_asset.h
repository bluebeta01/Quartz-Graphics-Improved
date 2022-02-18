#pragma once
#include <string>
#include <memory>

namespace Quartz
{
enum class AssetType
{
	NONE,
	TEXT,
	TEXTURE2D,
	MATERIAL,
	MODEL,
	SHADER
};
enum class AssetLoadStatus
{
	NOT_LOADED,
	LOAD_REQUESTED,
	READY_FOR_GPU_UPLOAD,
	GPU_UPLOAD_REQUESTED,
	LOADED
};

class AssetManager;

class Asset
{
public:
	Asset(AssetType type, const std::string& name, const std::string& absolutePath) :
		m_type(type), m_name(name), m_absolutePath(absolutePath)
	{}
	virtual ~Asset() {}

	virtual bool dependenciesLoaded() const = 0;

	AssetType getType() const { return m_type; }
	const std::string& getName() const { return m_name; }
	const std::string& getAbsolutePath() const { return m_absolutePath; }
	AssetLoadStatus getLoadStatus() const { return m_loadStatus; }
	void setLoadStatus(AssetLoadStatus status) { m_loadStatus = status; }

protected:
	AssetType m_type = AssetType::NONE;
	std::string m_name;
	std::string m_absolutePath;
	AssetLoadStatus m_loadStatus = AssetLoadStatus::LOAD_REQUESTED;
	bool m_hasDependencies = false;
	virtual void load(std::shared_ptr<Asset> asset) = 0;
	virtual void loadDependencies() = 0;

	friend class AssetManager;
};
}