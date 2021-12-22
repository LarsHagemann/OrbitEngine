#pragma once
#include <Eigen/Dense>

namespace orbit
{

    using namespace Eigen;

    struct Vertex
    {
        Vector3f position;
		Vector3f normal;
		Vector3f tangent;
		Vector2f uv;
    };

}