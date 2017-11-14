#version 420

layout (location = 0) in vec3 position;
layout (location = 0) in vec2 texCoord;
layout (location = 0) in vec3 normal;

out vec2 texCoord0;
out vec3 normal0;
out vec3 worldPos0;

uniform mat4 world;
uniform mat4 viewProjection;

void main()
{
	gl_Position = viewProjection * world * vec4(position, 1.0);
	texCoord0 = texCoord;
	normal0 = (world * vec4(normal, 0.0)).xyz;
    worldPos0 = (world * vec4(position, 1.0)).xyz;
}
