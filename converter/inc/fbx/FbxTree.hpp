#pragma once
#include <filesystem>
#include <istream>
#include <ostream>
#include <string_view>
#include <vector>
#include <variant>
#include <cassert>

#include "zlib.h"

namespace orbtool
{

    namespace fs = std::filesystem;

	using FBXProperty = std::variant<
		int16_t,
		bool,
		int32_t,
		float,
		double,
		int64_t,
		std::string,
		std::vector<float>,
		std::vector<int32_t>,
		std::vector<double>,
		std::vector<int64_t>,
		std::vector<bool>,
		std::vector<uint8_t>>;

	struct FBXNode
	{
		std::string name;
		std::vector<FBXProperty> properties;
		std::vector<FBXNode> children;

        const FBXNode* FindChild(std::string_view childName, size_t nthChild = 0U) const;
	};

    class FbxTree
    {
    private:
        std::istream* m_stream;
        FBXNode m_root;
    private:
        bool ReadNode(FBXNode* node);
        void ReadProperty(FBXProperty* property);
        std::string ReadString(size_t length);
        void ReadString(std::string* str);
        size_t Position() { return static_cast<size_t>(m_stream->tellg()); }
        void PrintProperty(std::ostream* stream, const FBXProperty* property);
        template<typename T>
        T ReadPrimitive()
        {
            T t;
            ReadPrimitive(&t);
            return t;
        }
        template<typename T>
        void ReadPrimitive(T* t)
        {
            m_stream->read((char*)t, sizeof(T));
        }
        template<typename T>
        void InitVariant(FBXProperty* property)
        {
            *property = T();
        }
        template<typename T>
        void ReadArray(std::vector<T>* array)
        {
            auto length = ReadPrimitive<uint32_t>();
            auto encoding = ReadPrimitive<uint32_t>();
            auto compLen = ReadPrimitive<uint32_t>();
            auto stride = sizeof(T);

            array->resize(length);
            std::vector<uint8_t> data(compLen);

            m_stream->read((char*)data.data(), compLen);

            if (encoding == 1)
            {
                // Data is compressed
                auto data2 = data;
                uLongf destLen = static_cast<uLongf>(length * stride);
                data.resize(destLen);

                uncompress((Bytef*)data.data(), &destLen, (const Bytef*)data2.data(), compLen);
            }

            assert(data.size() == length * stride);
            // Possible byte swap if endianness doesn't fit

            for (auto i = 0u; i < length; ++i)
            {
                auto pos = i * stride;
                array->at(i) = *reinterpret_cast<T*>(data.data() + pos);
            }
        }
        template<size_t Length>
        bool ArrayCompare(std::string_view str, const char arr[Length])
        {
            if (str.size() != Length)
                return false;

            return std::memcmp(str.data(), arr, Length) == 0;
        }
        void PrintTree(std::ostream* stream, const FBXNode* node, unsigned offset = 0U);
    public:
        void PrintTree(std::ostream* stream);
        FbxTree(std::istream* stream);

        const FBXNode* GetRootNode() const { return &m_root; }
    };

}