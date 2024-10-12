#pragma once
#include <array>
#include <GL/glew.h>
struct Vec2
{
	float x, y;
};
struct Vec3
{
	float x, y, z;
};
struct Vec4
{
	float x, y, z, w;
};
struct VertexStructure
{
	Vec2 Position;
	Vec2 TexCoords;
	float TexID;
};

std::array<VertexStructure, 4> CreateQuad(float x, float y, float sizeOfQuad, float TextureID);