#include "Pch.hpp"

#include "Core/Mesh.hpp"

namespace GFMDLConv
{

	void RawMesh::Destroy(RawMesh* mesh)
	{
		if (mesh == nullptr) return;

		for (auto submesh : mesh->RawSubmeshes)
		{
			if (submesh != nullptr)
				delete submesh;
		}

		mesh->RawSubmeshes.clear();
		delete mesh;
	}

}