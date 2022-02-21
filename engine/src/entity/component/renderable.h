#pragma once
#include <vector>
#include <memory>
#include <graphics.h>
#include "asset/assets/c_modelasset.h"

namespace Quartz
{
struct RenderableComponent
{
	std::shared_ptr<ModelAsset> model;
	std::vector<std::shared_ptr<CBuffer>> cbuffers;
};
}