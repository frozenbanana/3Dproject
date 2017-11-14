#version 420

in vec2 texCoord0;
in vec3 normal0;
in vec3 worldPos0;

layout (location = 0) out vec3 worldPos1;
layout (location = 1) out vec3 diffuse1;
layout (location = 2) out vec3 normal1;
layout (location = 3) out vec3 texCoord1;

uniform sampler2D gColorMap;

void main()
{
	worldPos1 = worldPos0;
	diffuse1 = texture(gColorMap, texCoord0).xyz;
	normal1 = normalize(normal0);
	texCoord1 = vec3(texCoord0, 0.0);
}
