#pragma once
#include "Engine/Misc/Helper.hpp"

namespace orbit
{

	// @brief: the type of light.
	enum class LightType
	{
		// @brief: a directional light is only defined by the direction and the color of the light
		//	the sun is a good example of a directional light. We do not care for the position of the sun,
		//	only the direction of its light rays matters
		// This is the fastest light to compute
		DirectionalLight,
		// @brief: a point light is a perfect light bulb, where you have a light source at a specific position
		//	and rays are going of in all directions (in contrast to the directional light)
		// Slower to compute than the directional light
		PointLight,
		// @brief: a spot light can be thought of as a flash light, where you have not a sphere of light
		//	but only a cone of light being emitted from a certain point with a certain angle
		// Spot lights are the most expensive lights to compute
		SpotLight
	};

	// @brief: holds information about a light in the scene
	//	therefore this struct holds information that all of the light types need
	struct Light
	{
		// @member: the color of the light. All lights share this property
		Vector4f _color = Vector4f::Ones();
		// @member: the position of the light. Only point- and spotlights
		Vector4f _position = Vector4f::Zero();
		// @member: the direction of the light rays. Only directional- and spotlights
		Vector4f _direction = Vector4f::Zero();
		// @member: the angle of a spotlight cone. Only spotlights
		float _spotAngle = 0.f;
		// @member: the distance at which to begin dimming the light. Only point- and spotlights
		float _falloffBegin = 0.f;
		// @member: the maximum distance of a light source. Only point- and spotlights
		float _falloffEnd = 1.f;
		// @member: the type of the light
		LightType _type = LightType::PointLight;

		// @method: creates a new directional light
		// @param color: the color of the light
		// @param direction: the direction of the light
		// @return: a directional light that can be inserted into a scene
		static std::shared_ptr<Light> CreateDirectionalLight(
			const Vector4f& color,
			const Vector4f& direction
		);

		// @method: creates a new point light
		// @param color: the color of the light
		// @param position: the position of the light
		// @param falloffBegin: the distance at which to begin dimming the light
		// @param falloffEnd: the maximum distance of the light source's rays
		// @return: a point light that can be inserted into a scene
		static std::shared_ptr<Light> CreatePointLight(
			const Vector4f& color,
			const Vector4f& position,
			float falloffBegin,
			float falloffEnd
		);

		// @method: creates a new spot light
		// @param color: the color of the light
		// @param position: the position of the light
		// @param direction: the direction of the light
		// @param spotAngle: the angle of the light cone
		// @param falloffBegin: the distance at which to begin dimming the light
		// @param falloffEnd: the maximum distance of the light source's rays
		// @return: a point light that can be inserted into a scene
		static std::shared_ptr<Light> CreateSpotLight(
			const Vector4f& color,
			const Vector4f& position,
			const Vector4f& direction,
			float spotAngle,
			float falloffBegin,
			float falloffEnd
		);
	};

	using LightPtr = std::shared_ptr<Light>;

}