#include "CreateQuadDynamic.h"


std::array<VertexStructure, 4> CreateQuad(float x, float y, float sizeOfQuad, float TextureID)
{
	
	VertexStructure v0{};
	v0.Position = { x, y };
	v0.TexCoords = { 0.0f, 0.0f };
	v0.TexID = { TextureID };

	VertexStructure v1{};
	v1.Position = { x + sizeOfQuad, y };
	v1.TexCoords = { 1.0f, 0.0f };
	v1.TexID = { TextureID };

	VertexStructure v2{};
	v2.Position = { x + sizeOfQuad, y + sizeOfQuad };
	v2.TexCoords = { 1.0f,1.0f };
	v2.TexID = { TextureID };

	VertexStructure v3{};
	v3.Position = { x, y + sizeOfQuad };
	v3.TexCoords = { 0.0f,1.0f };
	v3.TexID = { TextureID };

	return { v0, v1, v2, v3 };
}