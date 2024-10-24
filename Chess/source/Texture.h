#pragma once

#include "Renderer.h"
#include <string>

class Texture
{
public:
	GLuint CreateTexture(const std::string& path);

	void Unbind() const;
};