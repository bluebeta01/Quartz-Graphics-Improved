#pragma once
#include <memory>
#include "graphicsresource.h"

class Device : public Resource
{
public:
	static std::shared_ptr<Device> create();
	virtual void waitForIdle() = 0;
};