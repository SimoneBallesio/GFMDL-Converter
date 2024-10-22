#include "Pch.hpp"

#include "Parsers/GfmdlParser.hpp"

#include <pugixml.hpp>

namespace GFMDLConv
{

	static uint32_t s_ComponentLut[] = { 0, 3, 2, 3, 3, 3, 4 };

	static bool ParseXMLResult(const pugi::xml_parse_result& result);
	static bool ParseTextureList(pugi::xml_node list, RawMesh* mesh);
	static bool ParseMaterials(pugi::xml_node list, RawMesh* mesh);
	static bool ParseRawSubmeshes(pugi::xml_node list, RawMesh* mesh);
	static bool ParseSkeleton(pugi::xml_node list, RawMesh* mesh);

	static constexpr MeshAttributeName ParseMeshAttributeName(const std::string_view& name);
	static constexpr uint32_t ParseMeshIndexAttributeName(const std::string_view& name);

	static bool PopulateVertexData(std::vector<float>& component, uint32_t size, std::stringstream& ss);
	static void PopulateIndicesData(std::vector<uint32_t>& indices, uint32_t size, std::stringstream& ss);

	GFMDLParser::~GFMDLParser()
	{
		for (auto m : m_RawMeshes)
			RawMesh::Destroy(m);

		m_RawMeshes.clear();
	}

	const std::vector<RawMesh*>& GFMDLParser::GetMeshes() const
	{
		return m_RawMeshes;
	}

	bool GFMDLParser::Parse(const char* path)
	{
		pugi::xml_document model;
		const pugi::xml_parse_result result = model.load_file(path);

		if (!ParseXMLResult(result))
			return false;

		pugi::xml_node root = model.child("GfModel");

		std::filesystem::path p(path);

		RawMesh* m = new RawMesh();
		m->Name = p.filename().c_str();
		m_RawMeshes.push_back(m);

		bool success = ParseTextureList(root.child("TextureList"), m);
		if (success) success = ParseMaterials(root.child("Materials"), m);
		if (success) success = ParseRawSubmeshes(root.child("Meshes"), m);
		if (success) success = ParseSkeleton(root.child("Skelton"), m);

		if (!success && !DestroyRawMesh(m))
			std::cerr << "ERROR: Unable to safely destroy Mesh data, please report this error.\n";

		return success;
	}

	bool GFMDLParser::DestroyRawMesh(RawMesh* mesh)
	{
		for (auto it = m_RawMeshes.begin(); it != m_RawMeshes.end(); it++)
		{
			if (*it == mesh)
			{
				m_RawMeshes.erase(it);
				RawMesh::Destroy(mesh);
				return true;
			}
		}

		return false;
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

	static bool ParseTextureList(pugi::xml_node list, RawMesh* mesh)
	{
		return true;
	}

	static bool ParseMaterials(pugi::xml_node list, RawMesh* mesh)
	{
		return true;
	}

	static bool ParseRawSubmeshes(pugi::xml_node list, RawMesh* mesh)
	{
		if (!list) return true;

		for (pugi::xml_node it = list.child("Mesh"); it; it = it.next_sibling("Mesh"))
		{
			const auto name = it.attribute("Name").value();

			pugi::xml_node vertices = it.child("VertexData");
			pugi::xml_node faces = it.child("Faces");

			if (std::distance(faces.children().begin(), faces.children().end()) == 0)
			{
				std::cout << "\nWARNING: Skipping submesh \"" << name << "\" as it doesn't contain any material data.\n";
				continue;
			}

			RawSubmesh* m = new RawSubmesh();
			m->Name = name;
			mesh->RawSubmeshes.push_back(m);

			std::cout << "\nSubmesh \"" << name << "\"\n";

			for (pugi::xml_node v = vertices.first_child(); v; v = v.next_sibling())
			{
				const std::string_view typeName(v.attribute("Usage").value());
				const MeshAttributeName type = ParseMeshAttributeName(typeName);

				uint32_t size(atoi(v.attribute("Size").value()));
				size *= s_ComponentLut[type];

				std::stringstream value(v.child_value());

				std::cout << " -- " << typeName << ": expected " << size << " float values\n";

				switch (type)
				{
					case MeshAttributeName::Position:
					{
						if (!PopulateVertexData(m->Position.Values, size, value))
							return false;

						break;
					}

					case MeshAttributeName::UV:
					{
						auto& uv = m->UV.emplace_back();
						if (!PopulateVertexData(uv.Values, size, value))
							return false;

						break;
					}

					case MeshAttributeName::Normal:
					{
						if (!PopulateVertexData(m->Normal.Values, size, value))
							return false;

						break;
					}

					case MeshAttributeName::Tangent:
					{
						if (!PopulateVertexData(m->Tangent.Values, size, value))
							return false;

						break;
					}

					case MeshAttributeName::BiNormal:
					{
						if (!PopulateVertexData(m->BiNormal.Values, size, value))
							return false;

						break;
					}

					case MeshAttributeName::Color:
					{
						if (!PopulateVertexData(m->Color.Values, size, value))
							return false;

						break;
					}

					default:
					{
						std::cout << "    (Skipping: unsupported)\n";
						break;
					}
				}
			}

			std::cout << " -- Parsing unique indices.\n";

			for (const auto& f : faces.children("Face"))
			{
				const std::string_view material(f.attribute("Material").value());
				const uint32_t count(atoi(f.attribute("IndexLength").value()));

				std::cout << " -- -- Material \"" << material << "\": expected " << count << " indices for each attribute\n";

				for (const auto& i : f.children("int"))
				{
					const std::string_view typeName(i.attribute("Usage").value());
					const uint32_t type = ParseMeshIndexAttributeName(typeName);

					std::vector<uint32_t> indices;
					std::stringstream value(i.child_value());

					PopulateIndicesData(indices, count, value);

					std::cout << " -- -- " << typeName << ": found " << indices.size() << " integer values. ";

					if (indices.size() == count)
						std::cout << "OK\n";

					else
					{
						std::cout << "ERROR\n";
						std::cerr << "       Possible mesh corruption!\n";

						return false;
					}

					if (type & (1 << MeshAttributeName::Position))
						m->Position.Indices.assign(indices.begin(), indices.end());

					if (type & (1 << MeshAttributeName::UV))
					{
						const char* name = i.attribute("Name").value();
						const uint32_t index = (name[strlen(name) - 1] - '0') - 1;

						if (m->UV.size() <= index)
						{
							std::cerr << "       ERROR: Indices specified for missing UV map " << index << '\n';
							return false;
						}

						m->UV[index].Indices.assign(indices.begin(), indices.end());
					}

					if (type & (1 << MeshAttributeName::Normal))
						m->Normal.Indices.assign(indices.begin(), indices.end());

					if (type & (1 << MeshAttributeName::Tangent))
						m->Tangent.Indices.assign(indices.begin(), indices.end());

					if (type & (1 << MeshAttributeName::BiNormal))
						m->BiNormal.Indices.assign(indices.begin(), indices.end());

					if (type & (1 << MeshAttributeName::Color))
						m->Color.Indices.assign(indices.begin(), indices.end());
				}
			}
		}

		return true;
	}

	static bool ParseSkeleton(pugi::xml_node list, RawMesh* mesh)
	{
		return true;
	}

	static constexpr MeshAttributeName ParseMeshAttributeName(const std::string_view& name)
	{
		if (name == "Position") return MeshAttributeName::Position;
		else if (name == "Uv") return MeshAttributeName::UV;
		else if (name == "Normal") return MeshAttributeName::Normal;
		else if (name == "Tangent") return MeshAttributeName::Tangent;
		else if (name == "BiNormal") return MeshAttributeName::BiNormal;
		else if (name == "Color") return MeshAttributeName::Color;

		return MeshAttributeName::Unsupported;
	}

	static constexpr uint32_t ParseMeshIndexAttributeName(const std::string_view& name)
	{
		if (name == "PositionJointWeightIndex")
			return (1 << MeshAttributeName::Position); // | (1 << MeshAttributeName::JointWeight); // TODO

		else if (name == "UvIndex")
			return (1 << MeshAttributeName::UV);

		else if (name == "NormalIndex")
			return (1 << MeshAttributeName::Normal);

		else if (name == "TangentBiNormalIndex")
			return (1 << MeshAttributeName::Tangent) | (1 << MeshAttributeName::BiNormal);

		else if (name == "ColorIndex")
			return (1 << MeshAttributeName::Color);

		return MeshAttributeName::Unsupported;
	}

	static bool PopulateVertexData(std::vector<float>& component, uint32_t size, std::stringstream& ss)
	{
		component.reserve(size);

		do
		{
			float& f = component.emplace_back();
			ss >> f;

			if (ss.fail())
			{
				ss.clear();
				std::string token;
				ss >> token;
			}
		} while (!ss.eof());

		std::cout << " -- found " << component.size() << " float values. ";

		if (component.size() == size)
		{
			std::cout << "OK\n";
			return true;
		}

		std::cout << "ERROR\n";
		std::cerr << "    Possible mesh corruption!\n";

		return false;
	}

	static void PopulateIndicesData(std::vector<uint32_t>& indices, uint32_t size, std::stringstream& ss)
	{
		indices.reserve(size);

		do
		{
			uint32_t& f = indices.emplace_back();
			ss >> f;

			if (ss.fail())
			{
				ss.clear();
				std::string token;
				ss >> token;
			}
		} while (!ss.eof());
	}

}