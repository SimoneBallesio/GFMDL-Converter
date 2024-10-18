#pragma once

#include "Core/Mesh.hpp"

namespace GFMDLConv
{

	class GFMDLParser final
	{
	public:
		GFMDLParser() = default;
		GFMDLParser(GFMDLParser&) = delete;

		~GFMDLParser() = default;

		bool Parse(const char* path);

		GFMDLParser& operator=(GFMDLParser&) = delete;

	private:
		std::vector<Mesh> m_Meshes;
	};

}