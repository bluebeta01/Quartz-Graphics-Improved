#pragma once
#include "resource.h"

class Device : public Resource
{
public:
	virtual void waitForIdle() = 0;
};