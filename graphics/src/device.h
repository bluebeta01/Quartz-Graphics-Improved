#pragma once
#include <memory>
#include "graphicsresource.h"

class Device : public Resource
{
public:
	std::shared_ptr<Device> create();
	virtual void waitForIdle() = 0;
};