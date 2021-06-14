#include "fbx/fbx_read.hpp"
#include <zlib.h>

#include <iostream>
#include <cassert>

namespace fbx
{

	bool ReadNode(FBXNode* node, std::ifstream* stream);
	void ReadProperty(FBXProperty* property, std::ifstream* stream);
	std::string ReadString(std::ifstream* stream, size_t length);
	void ReadString(std::ifstream* stream, std::string* str);
	size_t Position(std::istream* stream) { return static_cast<size_t>(stream->tellg()); }
	void PrintProperty(const FBXProperty* property);
	template<typename T>
	T ReadPrimitive(std::ifstream* stream);
	template<typename T>
	void ReadPrimitive(std::ifstream* stream, T* t);
	template<typename T>
	void InitVariant(FBXProperty* property);
	template<typename T>
	void ReadArray(std::ifstream* stream, std::vector<T>* array);
	template<size_t Length>
	bool ArrayCompare(std::string_view str, const char arr[Length]);

	template<typename T>
	static std::ostream& operator<<(std::ostream& out, std::vector<T> vec)
	{
#ifdef _DEBUG
		for (auto i = 0u; i < vec.size(); ++i)
		{
			out << vec[i];
			if (i != vec.size() - 1)
				out << ", ";
		}
		return out;
#else
		return out << "ARRAY<" << typeid(T).name() << ">";
#endif
	}

	void PrintTree(const FBXNode* node, unsigned offset)
	{
		for (auto i = 0u; i < offset; ++i)
			std::cout << "  ";
		std::cout << '#' << node->name;
		if (node->properties.size() > 0)
		{
			std::cout << "{ ";
			for (auto i = 0u; i < node->properties.size(); ++i)
			{
				const auto& property = node->properties[i];
				PrintProperty(&property);
				if (i != node->properties.size() - 1)
					std::cout << ", ";
			}
			std::cout << " }";
		}
		std::cout << '\n';
		for (const auto& child : node->children)
			PrintTree(&child, offset + 1);
	}

	void GenerateFBXTree(FBXNode* root, const fs::path& filepath)
	{
		std::ifstream input(filepath, std::ios::binary | std::ios::in);
		GenerateFBXTree(root, &input);
	}

	void GenerateFBXTree(FBXNode* root, std::ifstream* stream)
	{
		static constexpr auto header_size = 23U;
		static char header[header_size] = "Kaydara FBX Binary\x20\x20\x00\x1a";

		if (!ArrayCompare<header_size>(ReadString(stream, header_size), header))
		{
			std::cout << "FBX Header does not match!\n";
			return;
		}

		auto version = ReadPrimitive<uint32_t>(stream);

		FBXNode node;
		while (ReadNode(&node, stream))
			root->children.emplace_back(node);
	}

	const FBXNode* FindChild(const FBXNode* parent, std::string_view childName, size_t nthChild)
	{
		auto i = 0u;
		for (const auto& child : parent->children)
		{
			if (child.name == childName)
			{
				if (i++ == nthChild)
					return &child;
			}
		}
		return nullptr;
	}

	std::string ReadString(std::ifstream* stream, size_t length)
	{
		std::string str;
		str.resize(length);
		stream->read(str.data(), length);
		return str;
	}

	void ReadString(std::ifstream* stream, std::string* str)
	{
		*str = ReadString(stream, ReadPrimitive<uint32_t>(stream));
	}

	void PrintProperty(const FBXProperty* property)
	{
		std::visit([](const auto& value) { 
				using T = std::decay_t<decltype(value)>;
				if constexpr (std::is_same<T, std::string>::value)
					std::cout << '"' << value << '"';
				else
					std::cout << "[" << typeid(T).name() << "] " << value;
			}, 
			*property);
	}

	bool ReadNode(FBXNode* node, std::ifstream* stream)
	{
		static constexpr auto sentinal_size = 13U;
		static const char sentinal[sentinal_size] = { "\x00" };

		auto offset = ReadPrimitive<uint32_t>(stream);
		if (offset == 0)
			return false;

		node->properties.resize(ReadPrimitive<uint32_t>(stream));
		auto propertyListLen = ReadPrimitive<uint32_t>(stream);
		node->name.resize(static_cast<uint32_t>(ReadPrimitive<uint8_t>(stream)));
		node->name = ReadString(stream, node->name.size());

		for (auto i = 0u; i < node->properties.size(); ++i)
			ReadProperty(&node->properties[i], stream);

		if (Position(stream) < offset)
		{
			// There is a nested offset list
			while (Position(stream) < (offset - sentinal_size))
			{
				node->children.push_back(FBXNode());
				ReadNode(&node->children.back(), stream);
			}

			if (!ArrayCompare<sentinal_size>(ReadString(stream, sentinal_size), sentinal))
				throw "Corrupted file. Expected 13 zero bytes";
		}

		if (Position(stream) != offset)
			throw "Expected end of object data";

		return true;
	}

	void ReadProperty(FBXProperty* property, std::ifstream* stream)
	{
		auto data_type = ReadPrimitive<char>(stream);
		switch (data_type)
		{
		case 'Y': { using T = int16_t             ; InitVariant<T>(property); ReadPrimitive(stream, &std::get<T>(*property)); break; }
		case 'C': { using T = bool                ; InitVariant<T>(property); ReadPrimitive(stream, &std::get<T>(*property)); break; }
		case 'I': { using T = int32_t             ; InitVariant<T>(property); ReadPrimitive(stream, &std::get<T>(*property)); break; }
		case 'F': { using T = float               ; InitVariant<T>(property); ReadPrimitive(stream, &std::get<T>(*property)); break; }
		case 'D': { using T = double              ; InitVariant<T>(property); ReadPrimitive(stream, &std::get<T>(*property)); break; }
		case 'L': { using T = int64_t             ; InitVariant<T>(property); ReadPrimitive(stream, &std::get<T>(*property)); break; }
		case 'R': { using T = std::string         ; InitVariant<T>(property); ReadString   (stream, &std::get<T>(*property)); break; }
		case 'S': { using T = std::string         ; InitVariant<T>(property); ReadString   (stream, &std::get<T>(*property)); break; }
		case 'f': { using T = std::vector<float>  ; InitVariant<T>(property); ReadArray    (stream, &std::get<T>(*property)); break; }
		case 'i': { using T = std::vector<int32_t>; InitVariant<T>(property); ReadArray    (stream, &std::get<T>(*property)); break; }
		case 'd': { using T = std::vector<double> ; InitVariant<T>(property); ReadArray    (stream, &std::get<T>(*property)); break; }
		case 'l': { using T = std::vector<int64_t>; InitVariant<T>(property); ReadArray    (stream, &std::get<T>(*property)); break; }
		case 'b': { using T = std::vector<bool>   ; InitVariant<T>(property); ReadArray    (stream, &std::get<T>(*property)); break; }
		case 'c': { using T = std::vector<uint8_t>; InitVariant<T>(property); ReadArray    (stream, &std::get<T>(*property)); break; }
		default:
			throw "Invalid data type identifier";
		}
	}

	template<typename T>
	T ReadPrimitive(std::ifstream* stream)
	{
		T t;
		stream->read(reinterpret_cast<char*>(&t), sizeof(T));
		return t;
	}

	template<typename T>
	void ReadPrimitive(std::ifstream* stream, T* t)
	{
		*t = ReadPrimitive<T>(stream);
	}

	template<typename T>
	void InitVariant(FBXProperty* property)
	{
		*property = T();
	}

	template<typename T>
	void ReadArray(std::ifstream* stream, std::vector<T>* array)
	{
		auto length = ReadPrimitive<uint32_t>(stream);
		auto encoding = ReadPrimitive<uint32_t>(stream);
		auto compLen = ReadPrimitive<uint32_t>(stream);
		auto stride = sizeof(T);

		array->resize(length);
		std::vector<uint8_t> data(compLen);

		stream->read((char*)data.data(), compLen);

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

		for (auto i = 0u; i < Length; ++i)
		{
			if (str[i] != arr[i])
				return false;
		}

		return true;
	}

}