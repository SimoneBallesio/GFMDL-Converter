#include "Pch.hpp"

#include "Core/Mesh.hpp"

#include "Output/Converter.hpp"

#include "Output/Formats/Wavefront.hpp"

namespace GFMDLConv
{

	Converter::Converter(const std::vector<RawMesh*> meshes, OutputFormat format, const std::string_view& path)
	{
		switch (format)
		{
			case OutputFormat::WavefrontObj:
			{
				m_ConversionOK = ToObjFormat(meshes, path);
				break;
			}

			default:
			{
				m_ConversionOK = false;
				return;
			}
		}
	}

	bool Converter::IsOK() const
	{
		return m_ConversionOK;
	}

}