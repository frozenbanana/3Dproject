#version 420

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

void processPrimitive();
bool cullPrimitive();

void main(){
	if(cullPrimitive()){
		processPrimitive();
	}
}

void processPrimitive(){
	mat4 PV = perspective * view;

	for(int i = 0; i < gl_in.length(); i++){
		gPos_W = gl_in[i].gl_Position;							//Save the position of a primitive in world-space

		gl_Position = PV * gPos_W;								//Go all the way and put it in view- and perspective-space

		gNormal_W = vNormal_W[i];
		//gNormal_W = camPos;

		gTex = vTex[i];

		
		//gTBN = vTBN[i];

		//gDif = vDif[i];
		//gSpe = vSpe[i];
		//gAmb = vAmb[i];

		EmitVertex();
	}
	EndPrimitive();

}

bool cullPrimitive(){

	bool doCull = true;		//A variable to hold the return value

	vec3 cornerVer;			//The vertex that is placed in the corner of the triangle
	vec3 edge1;				//First edge of the triangle
	vec3 edge2;				//Second edge of the triangle
	vec3 cullNorm;			//The normal of the triangle
	vec3 cullCam;			//The vector from the corner to the camera
	float coe;				//A coefficient holder

	cornerVer = vec3(gl_in[1].gl_Position);
	
	edge1 = vec3(gl_in[0].gl_Position) - cornerVer;
	edge2 = vec3(gl_in[2].gl_Position) - cornerVer;

	cullNorm = normalize(cross(edge1, edge2));

	cullCam = normalize(camPos - cornerVer);

	coe = dot(cullNorm, cullCam);

	if(coe > 0.0f){
		//If the dot-product is positive the triangle should not be culled
		doCull = false;
	}

	return doCull;
}