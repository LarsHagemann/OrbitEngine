#include "light.hpp"

namespace orbit
{

	std::shared_ptr<Light> Light::CreateDirectionalLight(const Vector4f& color, const Vector4f& direction)
	{
		auto light = std::make_shared<Light>();
		light->_type = LightType::DirectionalLight;
		light->_color = color;
		light->_direction = direction;
		return light;
	}

	std::shared_ptr<Light> Light::CreatePointLight(const Vector4f& color, const Vector4f& position, float falloffBegin, float falloffEnd)
	{
		auto light = std::make_shared<Light>();
		light->_type = LightType::PointLight;
		light->_color = color;
		light->_position = position;
		light->_falloffBegin = falloffBegin;
		light->_falloffEnd = falloffEnd;
		return light;
	}

	std::shared_ptr<Light> Light::CreateSpotLight(const Vector4f& color, const Vector4f& position, const Vector4f& direction, float spotAngle, float falloffBegin, float falloffEnd)
	{
		auto light = std::make_shared<Light>();
		light->_type = LightType::SpotLight;
		light->_color = color;
		light->_position = position;
		light->_direction = direction;
		light->_spotAngle = spotAngle;
		light->_falloffBegin = falloffBegin;
		light->_falloffEnd = falloffEnd;
		return light;
	}

}