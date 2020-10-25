#pragma once
#include "Engine/Management/AllocatorPage.hpp"

#include <vector>

namespace orbit
{

	// @brief: this struct defines generic vertex data being
	//	used for meshes.
	// @example:
	//	struct Vertex
	//	{
	//		XMFLOAT3 position;
	//		XMFLOAT3 normal;
	//		XMFLOAT3 tangent;
	//		XMFLOAT2 uv;
	//	};
	//	
	//	auto numVertices = 8u;
	//	auto stride = sizeof(Vertex);
	//
	//	auto data = _allocator.Allocate(numVertices * stride); // CPU memory allocator
	//	// fill data...
	//	
	//	auto vData = VertexData(stride, numVertices, data);
	//	// use vData for geometries...
	class VertexData
	{
	protected:
		// @member: the raw vertex data
		Allocation _data;
		// @member: size of a single vertex
		size_t _stride;
		// @member: number of vertices in the buffer
		size_t _numVertices;
	public:
		// constructs an empty VertexData container
		VertexData() : _stride(0), _numVertices(0), _data({}) {}
		// constructs a VertexData container
		VertexData(size_t stride, size_t numVertices, const Allocation& data) :
			_stride(stride),
			_numVertices(numVertices),
			_data(data) {}
		size_t GetStride() const { return _stride; }
		size_t GetVertexCount() const { return _numVertices; }
		const Allocation& GetBuffer() const { return _data; }
		Allocation& GetBuffer() { return _data; }
		size_t GetBufferSize() const { return _stride * _numVertices; }

		void SetStride(size_t stride) { _stride = stride; }
		void SetVertexCount(size_t numVertices) { _numVertices = numVertices; }
		void SetBuffer(const Allocation& buffer, size_t stride, size_t numVertices)
		{
			_data = buffer;
			_stride = stride;
			_numVertices = numVertices;
		}

	};

	struct IndexData
	{
		std::vector<uint16_t> indices;
		bool is16Bit = true;
		size_t size() const 
		{
			return is16Bit ? indices.size() : indices.size() / 2;
		}
		uint32_t at(size_t idx) const
		{
			if (is16Bit) idx *= 2;
			auto rawData = (uint32_t*)(indices.data() + idx);
			if (is16Bit) return *(uint16_t*)rawData;
			return *rawData;
		}
	};	

	static IndexData To32BitIndexData(const IndexData& in)
	{
		if (in.is16Bit) return in;

		IndexData result;
		result.is16Bit = false;
		result.indices.resize(in.indices.size() * 2); // every previous index takes twice the size

		for (auto i = 0u; i < result.indices.size(); ++i)
		{
			auto valueBegin = (uint32_t*)result.indices.data() + ((uint32_t)i * 2);
			*valueBegin = (uint32_t)in.indices.at(i);
		}

		return result;
	}

	static void CombineIndexData(IndexData& inOut, const IndexData& in, size_t preCombineVertexCount)
	{
		if (!in.is16Bit &&  inOut.is16Bit)
			inOut = To32BitIndexData(inOut);

		auto preIndexCount = inOut.indices.size();

		if (inOut.is16Bit == in.is16Bit)
		{
			inOut.indices.insert(
				inOut.indices.end(),
				in.indices.begin(),
				in.indices.end()
			);
		}
		else
		{
			auto copy = To32BitIndexData(in);
			inOut.indices.insert(
				inOut.indices.end(),
				copy.indices.begin(),
				copy.indices.end()
			);
		}

		if (inOut.is16Bit)
			std::transform(
				inOut.indices.begin() + preIndexCount,
				inOut.indices.end(),
				inOut.indices.begin() + preIndexCount,
				[preCombineVertexCount](const uint16_t& in) { return in + static_cast<uint16_t>(preCombineVertexCount); }
			);
		else
		{
			for (auto i = 0u; i < inOut.indices.size(); ++i)
			{
				auto valueBegin = (uint32_t*)inOut.indices.data() + (i * 2);
				*valueBegin += static_cast<uint32_t>(preCombineVertexCount);
			}
		}
	}

}