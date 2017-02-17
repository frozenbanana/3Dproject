#version 440
//in vec3 gcolor;
in vec2 gtex;

out vec4 fragment_color;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

in vec4 gdifcol;
in vec4 gspecol;
in vec4 gambcol;

void main(){
	//fragment_color = vec4(gcolor, 1.0f);
	//fragment_color = vec4(texture(texture_diffuse1, gtex));
	//fragment_color = vec4(texture(texture_specular1, gtex));
	//fragment_color = vec4(gtex, 1.0f, 0.0f);
	fragment_color = gdifcol;
}