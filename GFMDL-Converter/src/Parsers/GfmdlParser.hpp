#pragma once

#include "Core/Mesh.hpp"

namespace GFMDLConv
{

	class GFMDLParser final
	{
	public:
		GFMDLParser() = default;
		GFMDLParser(GFMDLParser&) = delete;

		~GFMDLParser();

		const std::vector<RawMesh*>& GetMeshes() const;

		bool Parse(const char* path);

		GFMDLParser& operator=(GFMDLParser&) = delete;

	private:
		std::vector<RawMesh*> m_RawMeshes;

		bool DestroyRawMesh(RawMesh* mesh);
	};

}