#pragma once

namespace GFMDLConv
{

	union Vec2
	{
		float v[2];

		struct
		{
			float x;
			float y;
		};

		inline float& operator[](int id) { return v[id]; }
		inline float operator[](int id) const { return v[id]; }
	};

	union Vec3
	{
		float v[3];

		struct
		{
			float x;
			float y;
			float z;
		};

		inline float& operator[](int id) { return v[id]; }
		inline float operator[](int id) const { return v[id]; }
	};

	union Vec4
	{
		float v[4];

		struct
		{
			float x;
			float y;
			float z;
			float w;
		};

		inline float& operator[](int id) { return v[id]; }
		inline float operator[](int id) const { return v[id]; }
	};

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

	struct Mesh
	{
		std::vector<Vec3> Position = {};
		std::vector<Vec2> UV = {};
		std::vector<Vec3> Normal = {};
		std::vector<Vec3> Tangent = {};
		std::vector<Vec3> BiNormal = {};
		std::vector<Vec4> Color = {};
	};

}