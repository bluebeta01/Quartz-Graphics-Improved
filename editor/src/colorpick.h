#pragma once
#include <entity\entity.h>
#include <glm.hpp>
#include <optional>

namespace Quartz::Editor
{
class ColorPick
{
public:
	static std::optional<Entity> getColorPickEntity(int viewportWidth, int viewportHeight, int pickX, int pickY,
		const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
private:
	
};
}