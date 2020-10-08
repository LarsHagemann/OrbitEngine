#pragma once
#include "renderable.hpp"
#include "component.hpp"
#include "transform.hpp"
#include "mesh.hpp"

namespace orbit
{

	class BatchComponent : public Renderable, public Component
	{
	protected:
		// @member: the mesh
		std::shared_ptr<Mesh> _mesh;
		// @member: instances of the mesh
		std::vector<TransformPtr> _transforms;
		// @member: the instance buffer
		Ptr<ID3D12Resource> _instanceBuffer;
		// @member: tracks the capacity of the instance buffer
		uint32_t _instanceCapacity;
	public:
		virtual void Draw(Ptr<ID3D12GraphicsCommandList> renderTo) const override;
	};

}
