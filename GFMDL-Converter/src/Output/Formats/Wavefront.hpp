#pragma once

namespace GFMDLConv
{

	struct RawMesh;

	bool ToObjFormat(const std::vector<RawMesh*> meshes, const std::string_view& path);

}