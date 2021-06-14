#pragma once
#include <variant>
#include <vector>
#include <filesystem>
#include <fstream>
#include <string>

namespace fbx
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
	};

	void PrintTree(const FBXNode* node, unsigned offset = 0U);

	void GenerateFBXTree(FBXNode* root, const fs::path& filepath);
	void GenerateFBXTree(FBXNode* root, std::ifstream* stream);

	const FBXNode* FindChild(const FBXNode* parent, std::string_view childName, size_t nthChild = 0U);

} // namespace fbx