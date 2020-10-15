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
		mutable Ptr<ID3D12Resource> _instanceBuffer;
		// @member: tracks the capacity of the instance buffer
		uint32_t _instanceCapacity;
	protected:
		void ResizeBuffer();
	public:
		// @constructor
		BatchComponent(EnginePtr engine, ObjectPtr boundObject, std::shared_ptr<Mesh> mesh);
		// @brief: creates a new BatchComponent
		static std::shared_ptr<BatchComponent> create(EnginePtr engine, ObjectPtr boundObject, std::shared_ptr<Mesh> mesh);
		// @method: sets the mesh to be rendered
		void SetMesh(std::shared_ptr<Mesh> mesh) { _mesh = mesh; }
		// @method: draws the batch to the scene
		virtual void Draw(Ptr<ID3D12GraphicsCommandList> renderTo) const override;
		// @method: adds a transform to the batch
		// @param transform: the transform to be added
		// @return: same as param transform
		TransformPtr AddTransform(TransformPtr transform);
		// @method: adds a new transform to the batch
		// @return: the newly created transform
		TransformPtr AddTransform();
	};

}
