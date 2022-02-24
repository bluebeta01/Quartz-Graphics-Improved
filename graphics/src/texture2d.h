#pragma once
#include <memory>
#include <vector>
#include <string>
#include <utility>
//#include "device.h"
#include "graphicsresource.h"
class Device;

enum class TextureType
{
	NONE, DEPTH_STENCIL, IMAGE, RENDER_TARGET
};

struct Texture2DCreateInfo
{
	std::shared_ptr<Device> device;
	int width;
	int height;
	TextureType type;
	int arraySize;
	void* textureResource;
	bool createSrvForRenderTarget;
};

class Texture2D : Resource
{
public:
	Texture2D(const Texture2DCreateInfo& createInfo) :
		m_device(createInfo.device), m_width(createInfo.width),
		m_height(createInfo.height), m_type(createInfo.type), m_arraySize(createInfo.arraySize) {}
	virtual void release() = 0;

	static std::shared_ptr<Texture2D> create(const Texture2DCreateInfo& createInfo);
	std::shared_ptr<Device> getDevice() const { return m_device; }
	int getWidth() const { return m_width; }
	int getHeight() const { return m_height; }
	TextureType getType() const { return m_type; }
	int getArraySize() const { return m_arraySize; }

protected:
	std::shared_ptr<Device> m_device;
	int m_width;
	int m_height;
	TextureType m_type;
	int m_arraySize;
};