#include "Pch.hpp"

#include "Parsers/GfmdlParser.hpp"

#include <pugixml.hpp>

namespace GFMDLConv
{

	static bool ParseXMLResult(const pugi::xml_parse_result& result);
	static bool ParseTextureList(pugi::xml_node list);
	static bool ParseMaterials(pugi::xml_node list);
	static bool ParseMeshes(pugi::xml_node list);
	static bool ParseSkeleton(pugi::xml_node list);

	static constexpr MeshAttributeName ParseMeshAttributeName(const std::string_view& name);

	bool GFMDLParser::Parse(const char* path)
	{
		pugi::xml_document model;
		const pugi::xml_parse_result result = model.load_file(path);

		if (!ParseXMLResult(result))
			return false;

		pugi::xml_node root = model.child("GfModel");

		bool success = ParseTextureList(root.child("TextureList"));
		if (success) success = ParseMaterials(root.child("Materials"));
		if (success) success = ParseMeshes(root.child("Meshes"));
		if (success) success = ParseSkeleton(root.child("Skelton"));

		return true;
	}

	static bool ParseXMLResult(const pugi::xml_parse_result& result)
	{
		switch (result.status)
		{
		case pugi::status_ok:
		{
			return true;
		}

		case pugi::status_file_not_found:
		{
			std::cerr << "ERROR: model file not found. Aborting.\n";
			return false;
		}

		case pugi::status_io_error:
		{
			std::cerr << "ERROR: Unable to read file, it may be protected or in use. Aborting.\n";
			return false;
		}

		case pugi::status_out_of_memory:
		{
			std::cerr << "ERROR: The model file is too large, system out of memory. Aborting.\n";
			return false;
		}

		default:
		{
			std::cerr << "ERROR: The model file is corrupted or formatted improperly. Aborting.\n";
			return false;
		}
		}

		return false;
	}

	static bool ParseTextureList(pugi::xml_node list)
	{
		return true;
	}

	static bool ParseMaterials(pugi::xml_node list)
	{
		return true;
	}

	static bool ParseMeshes(pugi::xml_node list)
	{
		if (!list) return true;

		for (pugi::xml_node it = list.child("Mesh"); it; it = it.next_sibling("Mesh"))
		{
			const auto meshName = it.attribute("Name").value();

			std::cout << "MESH \"" << meshName << "\"\n";

			pugi::xml_node vertData = it.child("VertexData");
			pugi::xml_node faceData = it.child("Faces");

			for (pugi::xml_node attr = vertData.first_child(); attr; attr = attr.next_sibling())
			{
				const std::string_view typeStr(attr.attribute("Usage").value());
				const MeshAttributeName type = ParseMeshAttributeName(typeStr);

				if (type == MeshAttributeName::Unsupported) continue;

				std::stringstream sizeStr(attr.attribute("Size").value());
				uint32_t size;
				sizeStr >> size;

				std::cout << " -- " << typeStr << ": found " << size << '\n';

				switch (type)
				{
					case MeshAttributeName::Position:
					{
						break;
					}

					case MeshAttributeName::UV:
					{
						break;
					}

					case MeshAttributeName::Normal:
					{
						break;
					}

					case MeshAttributeName::Tangent:
					{
						break;
					}

					case MeshAttributeName::BiNormal:
					{
						break;
					}

					case MeshAttributeName::Color:
					{
						break;
					}
				}
			}
		}

		return true;
	}

	static bool ParseSkeleton(pugi::xml_node list)
	{
		return true;
	}

	static constexpr MeshAttributeName ParseMeshAttributeName(const std::string_view& name)
	{
		if (name == "Position") return MeshAttributeName::Position;
		else if (name == "UV") return MeshAttributeName::UV;
		else if (name == "Normal") return MeshAttributeName::Normal;
		else if (name == "Tangent") return MeshAttributeName::Tangent;
		else if (name == "BiNormal") return MeshAttributeName::BiNormal;
		else if (name == "Color") return MeshAttributeName::Color;

		return MeshAttributeName::Unsupported;
	}

}