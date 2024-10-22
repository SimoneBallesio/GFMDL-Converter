#pragma once

namespace GFMDLConv
{

	enum MeshAttributeName
	{
		Unsupported = 0,
		Position,
		UV,
		Normal,
		Tangent,
		BiNormal,
		Color,
	};

	struct RawAttribute
	{
		std::vector<float> Values = {};
		std::vector<uint32_t> Indices = {};
	};

	struct RawSubmesh
	{
		std::string Name = {};

		RawAttribute Position = {};
		std::vector<RawAttribute> UV = {};
		RawAttribute Normal = {};
		RawAttribute Tangent = {};
		RawAttribute BiNormal = {};
		RawAttribute Color = {};
	};

	struct RawMesh
	{
		std::string Name = {};
		std::vector<RawSubmesh*> RawSubmeshes = {};

		static void Destroy(RawMesh* mesh);
	};

}