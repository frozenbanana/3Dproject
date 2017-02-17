#version 440

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

//in vec3 vcolor[];
in vec2 vtex[];
in vec4 vdifcol[];
in vec4 vspecol[];
in vec4 vambcol[];

//out vec3 gcolor;
out vec2 gtex;
out vec4 gdifcol;
out vec4 gspecol;
out vec4 gambcol;


uniform mat4 world;
uniform mat4 view;
uniform mat4 perspective;

void main(){
	
	mat4 PVW = perspective * view * world;

	for(int i = 0; i < gl_in.length(); i++){
		gl_Position = PVW * gl_in[i].gl_Position;

		//gcolor = vcolor[i];
		gtex = vtex[i];

		gdifcol = vdifcol[i];
		gspecol = vspecol[i];
		gambcol = vambcol[i];

		EmitVertex();
	}
	EndPrimitive();

}