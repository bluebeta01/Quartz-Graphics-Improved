#pragma once
#include <memory>
#include "graphicsresource.h"
#include "texture2d.h"

class Device : public Resource
{
public:
	static std::shared_ptr<Device> create();
	virtual void waitForIdle() = 0;
	virtual void uploadTexture(std::shared_ptr<Texture2D> texture, void* data, int dataSize) = 0;
	virtual bool textureUploadReady() = 0;
};