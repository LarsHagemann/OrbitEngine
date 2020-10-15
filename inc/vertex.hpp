#pragma once
#include "helper.hpp"

namespace orbit
{

	// @brief: defines a single vertex
	struct Vertex
	{
		// @member: position of the vertex in 3d space
		Vector3f position;
		// @member: normal of the vertex
		Vector3f normal;
		// @member: tangent of the vertex
		Vector3f tangent;
		// @member: uv coordinates of the vertex
		Vector2f uv;
	};

}
