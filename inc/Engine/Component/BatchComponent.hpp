#pragma once
#include "Renderable.hpp"
#include "Component.hpp"
#include "Engine/Misc/Transform.hpp"
#include "Engine/Rendering/Mesh.hpp"

namespace orbit
{

	class BatchComponent : public Renderable, public Component
	{
	protected:
		// @member: the mesh
		std::shared_ptr<Mesh> _mesh;
		// @member: instances of the mesh
		std::vector<TransformPtr> _transforms;
		// @member: tracks the capacity of the instance buffer
		uint32_t _instanceCapacity;
	public:
		// @constructor
		BatchComponent(ObjectPtr boundObject, std::shared_ptr<Mesh> mesh);
		// @brief: creates a new BatchComponent
		static std::shared_ptr<BatchComponent> create(ObjectPtr boundObject, std::shared_ptr<Mesh> mesh);
		// @method: sets the mesh to be rendered
		void SetMesh(std::shared_ptr<Mesh> mesh) { _mesh = mesh; }
		// @return: Returns the underlying mesh
		std::shared_ptr<Mesh> GetMesh() const { return _mesh; }
		// @method: draws the batch to the scene
		virtual void Draw(Renderer* renderer) const override;
		// @method: adds a transform to the batch
		// @param transform: the transform to be added
		// @return: same as param transform
		TransformPtr AddTransform(TransformPtr transform);
		// @method: adds a new transform to the batch
		// @return: the newly created transform
		TransformPtr AddTransform();

		virtual void DebugGUI(std::shared_ptr<DebugObject> object) const override;
	};

}