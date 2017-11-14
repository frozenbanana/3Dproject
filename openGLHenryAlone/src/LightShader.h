#pragma once
#include "Shader.h"
#include "Light.h"



class LightShader :
	public Shader
{
public:
	LightShader(const std::string& fileName);
	void SetPointLight(const PointLight& light);
	bool InitPointLightUniforms();
	void SetGPositionTextureUnit(unsigned int textureUnit);
	void SetGDiffuseTextureUnit(unsigned int textureUnit);
	void SetGNormalTextureUnit(unsigned int textureUnit);
	void SetScreenSize(unsigned int width, unsigned int height);
	~LightShader();

private:

	bool InitLightUniforms();

	enum
	{
		/* Deferred rendering related uniforms */
		G_POS_U,
		G_DIF_U,
		G_NOR_U,
		G_SPEC_MATERIAL_U,
		G_SPEC_POWER_U,
		G_SCREENSIZE_U,
		G_DIR_LIGHT_U,
		G_POI_LIGHT_U,
		G_SPO_LIGHT_U,
		NUM_LIGHT_UNIFORMS
	};
	
	GLuint m_lightUniforms[NUM_LIGHT_UNIFORMS];
	/* Point light uniform struct */
	struct 
	{
		GLuint Color;
		GLuint AmbientIntensity;
		GLuint DiffuseIntensity;
		GLuint Position;
		struct {
			GLuint Constant;
			GLuint Linear;
			GLuint Quadratic;
		} Atten;
	} m_pointLightLocation;


};

