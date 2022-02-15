#pragma once
#include "graphicsresource.h"

class Device : public Resource
{
public:
	virtual void waitForIdle() = 0;
};