#include "fbx/FbxTree.hpp"
#include "implementation/misc/Logger.hpp"

namespace orbtool
{

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

    bool FbxTree::ReadNode(FBXNode* node)
    {
        static constexpr auto sentinal_size = 13U;
		static const char sentinal[sentinal_size] = { "\x00" };

		auto offset = ReadPrimitive<uint32_t>();
		if (offset == 0)
			return false;

		node->properties.resize(ReadPrimitive<uint32_t>());
		auto propertyListLen = ReadPrimitive<uint32_t>();
		node->name.resize(static_cast<uint32_t>(ReadPrimitive<uint8_t>()));
		node->name = ReadString(node->name.size());

		for (auto i = 0u; i < node->properties.size(); ++i)
			ReadProperty(&node->properties[i]);

		if (Position() < offset)
		{
			// There is a nested offset list
			while (Position() < (offset - sentinal_size))
			{
				node->children.push_back(FBXNode());
				ReadNode(&node->children.back());
			}

			if (!ArrayCompare<sentinal_size>(ReadString(sentinal_size), sentinal))
				throw "Corrupted file. Expected 13 zero bytes";
		}

		if (Position() != offset)
			throw "Expected end of object data";

		return true;
    }

    void FbxTree::ReadProperty(FBXProperty* property)
    {
        auto data_type = ReadPrimitive<char>();
		switch (data_type)
		{
		case 'Y': { using T = int16_t             ; InitVariant<T>(property); ReadPrimitive(&std::get<T>(*property)); break; }
		case 'C': { using T = bool                ; InitVariant<T>(property); ReadPrimitive(&std::get<T>(*property)); break; }
		case 'I': { using T = int32_t             ; InitVariant<T>(property); ReadPrimitive(&std::get<T>(*property)); break; }
		case 'F': { using T = float               ; InitVariant<T>(property); ReadPrimitive(&std::get<T>(*property)); break; }
		case 'D': { using T = double              ; InitVariant<T>(property); ReadPrimitive(&std::get<T>(*property)); break; }
		case 'L': { using T = int64_t             ; InitVariant<T>(property); ReadPrimitive(&std::get<T>(*property)); break; }
		case 'R': { using T = std::string         ; InitVariant<T>(property); ReadString   (&std::get<T>(*property)); break; }
		case 'S': { using T = std::string         ; InitVariant<T>(property); ReadString   (&std::get<T>(*property)); break; }
		case 'f': { using T = std::vector<float>  ; InitVariant<T>(property); ReadArray    (&std::get<T>(*property)); break; }
		case 'i': { using T = std::vector<int32_t>; InitVariant<T>(property); ReadArray    (&std::get<T>(*property)); break; }
		case 'd': { using T = std::vector<double> ; InitVariant<T>(property); ReadArray    (&std::get<T>(*property)); break; }
		case 'l': { using T = std::vector<int64_t>; InitVariant<T>(property); ReadArray    (&std::get<T>(*property)); break; }
		case 'b': { using T = std::vector<bool>   ; InitVariant<T>(property); ReadArray    (&std::get<T>(*property)); break; }
		case 'c': { using T = std::vector<uint8_t>; InitVariant<T>(property); ReadArray    (&std::get<T>(*property)); break; }
		default:
			throw "Invalid data type identifier";
		}
    }

    std::string FbxTree::ReadString(size_t length)
    {
        std::string str;
		str.resize(length);
		m_stream->read(str.data(), length);
		return str;
    }

    void FbxTree::ReadString(std::string* str)
    {
        *str = ReadString(ReadPrimitive<uint32_t>());
    }

    void FbxTree::PrintProperty(std::ostream* stream, const FBXProperty* property)
    {
        std::visit([&](const auto& value) { 
				using T = std::decay_t<decltype(value)>;
				if constexpr (std::is_same<T, std::string>::value)
					*stream << '"' << value << '"';
				else
					(*stream) << "[" << typeid(T).name() << "] " << value;
			}, 
			*property);
    }

    FbxTree::FbxTree(std::istream* stream) : 
        m_stream(stream)
    {
        static constexpr auto header_size = 23U;
		static char header[header_size] = "Kaydara FBX Binary\x20\x20\x00\x1a";

		if (!ArrayCompare<header_size>(ReadString(header_size), header))
		{
            ORBIT_ERROR("FBX Header does not match!");
			return;
		}

		auto version = ReadPrimitive<uint32_t>();

		FBXNode node;
		while (ReadNode(&node))
			m_root.children.emplace_back(node);
    }

    void FbxTree::PrintTree(std::ostream* stream, const FBXNode* node, unsigned offset)
    {
        for (auto i = 0u; i < offset; ++i)
			*stream << "  ";
		*stream << '#' << node->name;
		if (node->properties.size() > 0)
		{
			*stream << "{ ";
			for (auto i = 0u; i < node->properties.size(); ++i)
			{
				const auto& property = node->properties[i];
				PrintProperty(stream, &property);
				if (i != node->properties.size() - 1)
					*stream << ", ";
			}
			*stream << " }";
		}
		*stream << '\n';
		for (const auto& child : node->children)
			PrintTree(stream, &child, offset + 1);
    }

    void FbxTree::PrintTree(std::ostream* stream)
    {
        PrintTree(stream, &m_root);
    }

    const FBXNode* FBXNode::FindChild(std::string_view childName, size_t nthChild) const
    {
        auto i = 0u;
		for (const auto& child : children)
		{
			if (child.name == childName)
			{
				if (i++ == nthChild)
					return &child;
			}
		}
		return nullptr;
    }

}
