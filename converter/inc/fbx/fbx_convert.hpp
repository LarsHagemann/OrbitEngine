#pragma once
#include "fbx_read.hpp"
#include "../orb/orb.hpp"
#include "Eigen/Dense"

#include <unordered_map>

namespace fbx
{

	using namespace Eigen;

	bool FBXToOrb(const FBXNode* rootNode, orbit::Orb* orb);

}
