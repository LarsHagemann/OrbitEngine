#include "batch_component.hpp"
#include "engine.hpp"

namespace orbit
{

	void BatchComponent::Draw(Ptr<ID3D12GraphicsCommandList> renderTo) const
	{
		D3D12_VERTEX_BUFFER_VIEW views[2] = {
			{ 0, sizeof(Vertex), 0U },
			{ 0, sizeof(Matrix4f), 0U } 
		};

		std::vector<Matrix4f> objects;
		objects.reserve(_transforms.size());
		for (const auto& t : _transforms)
			objects.emplace_back(t->LocalToWorldMatrix());

		auto instanceCount = objects.size();
		auto copySize = sizeof(Matrix4f) * instanceCount;

		if (_instanceBuffer)
		{
			void* buffer = nullptr;
			if (SUCCEEDED(_instanceBuffer->Map(0, nullptr, &buffer)))
			{
				memcpy(buffer, objects.data(), copySize);
				_instanceBuffer->Unmap(0, nullptr);
			}
		}

		renderTo->IASetVertexBuffers(0, 2, views);
		//if (_mesh->GetIndices().size() > 0)
		//	renderTo->IASetIndexBuffer(&_mesh->GetIndexBufferView());

		auto instances = _transforms.size();
		const auto& submeshes = _mesh->GetSubmeshes();
		for (const auto& submesh : submeshes)
		{
			_engine->BindPipelineState(submesh.pipelineState);
			if (!submesh.material.empty())
				_engine->GetMaterial(submesh.material)->BindMaterial();

			if (submesh.indexCount > 0)
				renderTo->DrawIndexedInstanced(submesh.indexCount, instances, submesh.startIndex, submesh.startVertex, 0);
			else
				renderTo->DrawInstanced(submesh.vertexCount, instances, submesh.startVertex, 0);
		}
	}

}