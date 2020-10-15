#include "batch_component.hpp"
#include "engine.hpp"
#include "exception.hpp"

namespace orbit
{
	void BatchComponent::ResizeBuffer()
	{
		if (_instanceCapacity >= _transforms.size())
			return;
		if (_instanceCapacity == 0)
			_instanceCapacity = 1;

		_instanceCapacity *= 2;

		ORBIT_INFO_LEVEL(FormatString("Resizing instance buffer %d -> %d.", _instanceCapacity / 2, _instanceCapacity), 3);

		auto cq = _engine->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
		auto ccl = cq->GetCommandList();

		std::vector<Matrix4f> objects;
		objects.reserve(_transforms.size());
		for (auto t : _transforms)
			objects.emplace_back(t->LocalToWorldMatrix());

		auto instanceCount = objects.size();
		auto copySize = sizeof(Matrix4f) * instanceCount;

		Ptr<ID3D12Resource> intermediateInstanceBuffer;
		UpdateBufferResource(
			_engine->GetDevice(),
			ccl.Get(),
			_instanceBuffer.GetAddressOf(),
			intermediateInstanceBuffer.GetAddressOf(),
			instanceCount,
			sizeof(Vertex),
			objects.data()
		);

		ORBIT_INFO(FormatString("Acquiring %d bytes of memory", copySize));
	}

	BatchComponent::BatchComponent(EnginePtr engine, ObjectPtr boundObject, std::shared_ptr<Mesh> mesh) :
		Component(engine, boundObject),
		_mesh(mesh),
		_instanceCapacity(0)
	{
		ORBIT_INFO_LEVEL(FormatString("Creating BatchComponent."), 5);
	}

	std::shared_ptr<BatchComponent> BatchComponent::create(EnginePtr engine, ObjectPtr boundObject, std::shared_ptr<Mesh> mesh)
	{
		return std::make_shared<BatchComponent>(engine, boundObject, mesh);
	}

	void BatchComponent::Draw(Ptr<ID3D12GraphicsCommandList> renderTo) const
	{
		if (_transforms.empty()) return;

		std::vector<Matrix4f> objects;
		objects.reserve(_transforms.size());
		for (const auto& t : _transforms)
			objects.emplace_back(t->LocalToWorldMatrix());

		auto instanceCount = objects.size();
		auto copySize = sizeof(Matrix4f) * instanceCount;

		auto cq = _engine->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
		auto ccl = cq->GetCommandList();

		Ptr<ID3D12Resource> intermediateInstanceBuffer;
		UpdateBufferResource(
			_engine->GetDevice(),
			ccl.Get(),
			_instanceBuffer.GetAddressOf(),
			intermediateInstanceBuffer.GetAddressOf(),
			instanceCount,
			sizeof(Vertex),
			objects.data()
		);

		D3D12_VERTEX_BUFFER_VIEW instanceBufferView;
		instanceBufferView.BufferLocation = _instanceBuffer->GetGPUVirtualAddress();
		instanceBufferView.SizeInBytes = copySize;
		instanceBufferView.StrideInBytes = sizeof(Matrix4f);

		D3D12_VERTEX_BUFFER_VIEW views[2] = {
			_mesh->GetVertexBufferView(),
			instanceBufferView
		};

		auto fence = cq->ExecuteCommandList(ccl);
		cq->WaitForFenceValue(fence);

		renderTo->IASetVertexBuffers(0, 2, views);
		if (_mesh->GetIndices().size() > 0)
			renderTo->IASetIndexBuffer(&_mesh->GetIndexBufferView());

		auto instances = _transforms.size();
		const auto& submeshes = _mesh->GetSubmeshes();
		for (const auto& submesh : submeshes)
		{
			_engine->BindPipelineState(submesh.pipelineState);
			if (!submesh.material.empty())
				_engine->GetMaterial(submesh.material)->BindMaterial(renderTo);

			if (submesh.indexCount > 0)
				renderTo->DrawIndexedInstanced(submesh.indexCount, instances, submesh.startIndex, submesh.startVertex, 0);
			else
				renderTo->DrawInstanced(submesh.vertexCount, instances, submesh.startVertex, 0);
		}
	}

	TransformPtr BatchComponent::AddTransform(TransformPtr transform)
	{
		ORBIT_INFO_LEVEL(FormatString("Adding transform to BatchComponent"), 10);
		_transforms.emplace_back(transform);
		ResizeBuffer();
		return transform;
	}

	TransformPtr BatchComponent::AddTransform()
	{
		auto transform = std::make_shared<Transform>();
		return AddTransform(transform);
	}

}