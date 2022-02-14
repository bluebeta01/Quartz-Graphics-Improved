#pragma once
#include <memory>
#include <vector>
#include <string>
#include <utility>
#include "device.h"
#include "descheap.h"

enum class TextureType
{
	DEPTH_STENCIL, IMAGE
};

struct Texture2DCreateInfo
{
	std::shared_ptr<Device> device;
	std::shared_ptr<DescriptorHeap> desciptorHeap;
	int width;
	int height;
	TextureType type;
	int arraySize;
	bool createTetureResource;
};

class Texture2D : Resource
{
public:
	Texture2D(const Texture2DCreateInfo& createInfo) :
		m_device(createInfo.device), m_desciptorHeap(createInfo.desciptorHeap), m_width(createInfo.width),
		m_height(createInfo.height), m_type(createInfo.type), m_arraySize(createInfo.arraySize) {}

	virtual void bufferData(void* data, int dataSize) = 0;

	std::shared_ptr<Device> getDevice() const { return m_device; }
	std::shared_ptr<DescriptorHeap> getDesciptorHeap() const { return m_desciptorHeap; }
	int getWidth() const { return m_width; }
	int getHeight() const { return m_height; }
	TextureType getType() const { return m_type; }
	int getArraySize() const { return m_arraySize; }

private:
	std::shared_ptr<Device> m_device;
	std::shared_ptr<DescriptorHeap> m_desciptorHeap;
	int m_width;
	int m_height;
	TextureType m_type;
	int m_arraySize;
};

typedef std::pair <std::string, std::shared_ptr<Texture2D>> NamedTexture;