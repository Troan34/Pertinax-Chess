#shader vertex
#version 450 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in float texIndex;

out vec2 v_TexCoord;
out float v_TexIndex;

uniform mat4 u_MVP;

void main()
{
   gl_Position = u_MVP * position;
   v_TexCoord = texCoords;
   v_TexIndex = texIndex;
};

#shader fragment
#version 450 core

layout(location = 0) out vec4 color; 

in vec2 v_TexCoord;
in float v_TexIndex;

uniform sampler2D u_Textures[15];


void main()
{
	int index = int(v_TexIndex);
	vec4 texColor = texture(u_Textures[index], v_TexCoord);
    color = texColor;
};
