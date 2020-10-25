#include "Engine/Component/BatchComponent.hpp"
#include "Engine/Misc/Logger.hpp"
#include "Engine/Engine.hpp"
#include "Engine/DebugObject.hpp"

#include "imgui.h"

namespace orbit
{

	BatchComponent::BatchComponent(ObjectPtr boundObject, std::shared_ptr<Mesh> mesh) :
		Component(boundObject),
		_mesh(mesh),
		_instanceCapacity(0)
	{
		ORBIT_INFO_LEVEL(FormatString("Creating BatchComponent."), 5);
        set_flag(RenderableFlag);
	}

	std::shared_ptr<BatchComponent> BatchComponent::create(ObjectPtr boundObject, std::shared_ptr<Mesh> mesh)
	{
		return std::make_shared<BatchComponent>(boundObject, mesh);
	}

	void BatchComponent::Draw(Renderer* renderer) const
	{
		if (_transforms.empty()) return;

        const auto instanceCount = _transforms.size();
        constexpr auto elementSize = sizeof(Matrix4f);
        auto bufferSize = elementSize * instanceCount;

        auto buffer = Engine::Get()->Allocate(bufferSize, 256);

        // copy transformation matrices into the upload buffer
        for (auto i = 0U; i < _transforms.size(); ++i)
        {
            const auto& t = _transforms[i];
            const auto& m = t->LocalToWorldMatrix();

            memcpy_s((uint8_t*)buffer.CPU + i * elementSize, elementSize, &m, elementSize);
        }

        VertexBufferView buffers[2] = {
            _mesh->GetVertexBufferView(),
            MakeVertexBufferView(buffer, bufferSize, elementSize)
        };

        renderer->BindVertexBuffers(0, 2, buffers);
        
        if (_mesh->GetIndexCount() > 0)
            renderer->BindIndexBuffer(_mesh->GetIndexBufferView());

        const auto& submeshes = _mesh->GetSubmeshes();
		for (const auto& submesh : submeshes)
		{
            renderer->BindPipelineState(submesh.pipelineState);
			renderer->BindRootSignature(submesh.pipelineState);
			if (!submesh.material.empty())
                renderer->BindMaterial(submesh.material);

			if (submesh.indexCount > 0)
				renderer->DrawIndexedInstanced(submesh.indexCount, instanceCount, submesh.startIndex, submesh.startVertex, 0);
			else
				renderer->DrawInstanced(submesh.vertexCount, instanceCount, submesh.startVertex, 0);
		}
	}

	TransformPtr BatchComponent::AddTransform(TransformPtr transform)
	{
		ORBIT_INFO_LEVEL(FormatString("Adding transform to BatchComponent"), 10);
		_transforms.emplace_back(transform);
		return transform;
	}

	TransformPtr BatchComponent::AddTransform()
	{
		auto transform = std::make_shared<Transform>();
		return AddTransform(transform);
	}

	void BatchComponent::DebugGUI(std::shared_ptr<DebugObject> object) const
	{
		if (ImGui::TreeNode(FormatString("Instances: %d", _transforms.size()).c_str()))
		{
			for (auto i = 0u; i < _transforms.size(); ++i)
				if (ImGui::TreeNode(FormatString("%d", i).c_str()))
				{
					object->ShowMatrix(_transforms[i]->LocalToWorldMatrix());
					ImGui::TreePop();
				}

			ImGui::TreePop();
		}
	}

}