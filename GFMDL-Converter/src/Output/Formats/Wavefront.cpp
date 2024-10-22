#include "Pch.hpp"

#include "Core/Mesh.hpp"

#include "Output/Formats/Wavefront.hpp"

namespace GFMDLConv
{

	bool ToObjFormat(const std::vector<RawMesh*> meshes, const std::string_view& path)
	{
		std::fstream f(path, std::ios::out | std::ios::beg | std::ios::trunc);

		if (!f.is_open())
		{
			std::cerr << "\nERROR: Unable to open output file path (" << path << ")\n";
			return false;
		}

		f << "# Generated with GFMDL-Converter\n";

		for (const auto m : meshes)
		{
			for (const auto b : m->RawSubmeshes)
			{
				f << "\ng " << m->Name << '_' << b->Name << "\n\n";

				const bool hasPositions = b->Position.Values.size() > 0;

				if (!hasPositions)
				{
					std::cerr << "    ERROR: No position data found for submesh \"" << b->Name << "\". Aborting.\"";
 					return false;
				}

				for (size_t i = 0; i < b->Position.Values.size(); i += 3)
					f << "\tv " << b->Position.Values[i] << ' ' << b->Position.Values[i + 1] << ' ' << b->Position.Values[i + 2] << '\n';

				const bool hasUV = b->UV.size() > 0 && b->UV[0].Values.size() > 0;

				if (hasUV)
				{
					f << '\n';

					for (size_t i = 0; i < b->UV[0].Values.size(); i += 2)
						f << "\tvt " << b->UV[0].Values[i] << ' ' << b->UV[0].Values[i + 1] << '\n';
				}

				const bool hasNormals = b->Normal.Values.size() > 0;

				if (hasNormals)
				{
					f << '\n';

					for (size_t i = 0; i < b->Normal.Values.size(); i += 3)
						f << "\tvn " << b->Normal.Values[i] << ' ' << b->Normal.Values[i + 1] << ' ' << b->Normal.Values[i + 2] << '\n';
				}

				const bool hasIndices = b->Position.Indices.size() > 0;

				if (!hasIndices)
				{
					std::cerr << "    WARNING: The submesh \"" << b->Name << "\" has no indices, skipping face generation.\n";
					continue;
				}

				const bool hasUvIndices = hasUV && b->UV[0].Indices.size() == b->Position.Indices.size();
				const bool hasNormalIndices = hasNormals && b->Normal.Indices.size() == b->Position.Indices.size();
				const bool hasSecondaryIndices = hasUvIndices || hasNormalIndices;

				for (size_t i = 0; i < b->Position.Indices.size(); i += 3)
				{
					f << "f " << (b->Position.Indices[i] + 1);

					if (hasSecondaryIndices)
					{
						f << '/';

						if (hasUvIndices)
							f << (b->UV[0].Indices[i] + 1);

						if (hasNormalIndices)
							f << '/' << (b->Normal.Indices[i] + 1);
					}

					f << " " << (b->Position.Indices[i + 1] + 1);

					if (hasSecondaryIndices)
					{
						f << '/';

						if (hasUvIndices)
							f << (b->UV[0].Indices[i + 1] + 1);

						if (hasNormalIndices)
							f << '/' << (b->Normal.Indices[i + 1] + 1);
					}

					f << " " << (b->Position.Indices[i + 2] + 1);

					if (hasSecondaryIndices)
					{
						f << '/';

						if (hasUvIndices)
							f << (b->UV[0].Indices[i + 2] + 1);

						if (hasNormalIndices)
							f << '/' << (b->Normal.Indices[i + 2] + 1);
					}

					f << '\n';
				}
			}
		}

		return true;
	}

}