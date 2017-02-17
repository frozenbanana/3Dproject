#version 440

//layout(location = 0) in vec3 vertex_position;
//layout(location = 1) in vec3 vertex_color;

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 texture_uvs;
layout(location = 3) in vec4 vertex_diffuse_color;
layout(location = 4) in vec4 vertex_specular_color;
layout(location = 5) in vec4 vertex_ambient_color;

//out vec3 vcolor;

out vec2 vtex;
out vec4 vdifcol;
out vec4 vspecol;
out vec4 vambcol;


void main(){
	gl_Position = vec4(vertex_position, 1.0);

	//vcolor = vertex_color;
	vtex = texture_uvs;
	vdifcol = vertex_diffuse_color;
	vspecol = vertex_specular_color;
	vambcol = vertex_ambient_color;

}

