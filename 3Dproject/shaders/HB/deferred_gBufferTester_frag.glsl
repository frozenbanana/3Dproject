#version 330 core
in vec2 TexCoords;
out vec4 FragColor;


uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

void main(){
	vec3 tempFragColor;
	if(TexCoords.x < 0.5 && TexCoords.y < 0.5)		// left bottom show Positions
	{
		tempFragColor = texture(gPosition, TexCoords).rgb;
	}

	if(TexCoords.x > 0.5 && TexCoords.y < 0.5)		// right bottom show normals
	{
		tempFragColor = texture(gNormal, TexCoords).rgb;
	}	

	if(TexCoords.x > 0.5 && TexCoords.y > 0.5)		// right top show diffuse color
	{
		tempFragColor = texture(gAlbedoSpec, TexCoords).rgb;
	}

	if(TexCoords.x < 0.5 && TexCoords.y > 0.5)
	{
		tempFragColor.x = 1.0 - texture(gAlbedoSpec, TexCoords).a;
		tempFragColor.y = tempFragColor.x;
		tempFragColor.z = tempFragColor.y;
	}

	FragColor = vec4(tempFragColor, 1.0);	
}

