//
//    #version 330 core
//	layout (location = 0) in vec3 pos;
//	layout (location = 1) in vec2 texCoord; //for texture coordinates 
//
//	out vec2 TextCoord;
//	void main()
//	{
//	   gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
//	   TextCoord = texCoord;
//	};
//

#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoord;

out vec2 TextCoord;

void main()
{
   // Set the homogeneous coordinate w to 1.0
   gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
   TextCoord = texCoord;
}
