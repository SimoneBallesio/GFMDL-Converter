#pragma once

enum OutputFormat
{
	Unsupported = 0,
	WavefrontObj,
};

namespace GFMDLConv
{

	struct RawMesh;

	class Converter final
	{
	public:
		Converter(const std::vector<RawMesh*> meshes, OutputFormat format, const std::string_view& path);
		Converter(Converter&) = delete;
		~Converter() = default;

		bool IsOK() const;

		Converter& operator=(Converter&) = delete;

	private:
		bool m_ConversionOK = false;

		Converter() = default;
	};

}