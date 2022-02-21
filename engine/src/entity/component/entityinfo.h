#pragma once
#include <string>
#include "transform.h"

namespace Quartz
{
struct EntityInfoComponent
{
	std::string name;
	Transform transform;
};
}