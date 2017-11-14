#include "LightShader.h"



LightShader::LightShader(const std::string& fileName) :
	Shader(fileName)
{
	if (!this->InitLightUniforms())
		std::cout << "Problem with InitLightUniforms()" << std::endl;
	if (!this->InitPointLightUniforms())
		std::cout << "Problem with InitPointLightUniforms()" << std::endl;
}


LightShader::~LightShader()
{
}

bool LightShader::InitLightUniforms()
{

	this->m_lightUniforms[G_POS_U] = glGetUniformLocation(this->m_program, "gPositionMap");
	this->m_lightUniforms[G_DIF_U] = glGetUniformLocation(this->m_program, "gDiffuseMap");
	this->m_lightUniforms[G_NOR_U] = glGetUniformLocation(this->m_program, "gNormalMap");
	this->m_lightUniforms[G_SPEC_MATERIAL_U] = glGetUniformLocation(this->m_program, "gMatSpecularIntensity");
	this->m_lightUniforms[G_SPEC_POWER_U] = glGetUniformLocation(this->m_program, "gSpecularPower");
	this->m_lightUniforms[G_DIR_LIGHT_U] = glGetUniformLocation(this->m_program, "directionalLightVec");
	this->m_lightUniforms[G_POI_LIGHT_U] = glGetUniformLocation(this->m_program, "gPointLight");
	this->m_lightUniforms[G_SPO_LIGHT_U] = glGetUniformLocation(this->m_program, "gSpotLight");
	this->m_lightUniforms[G_SCREENSIZE_U] = glGetUniformLocation(this->m_program, "gScreenSize");
		
	bool success = true;
	for (size_t i = 0; i < NUM_LIGHT_UNIFORMS; i++)
	{
		if (this->m_lightUniforms[i] == INVALID_UNIFORM_LOCATION)
		{
			success = false;
		}
	}

	return success;
	}

bool LightShader::InitPointLightUniforms()
{
	m_pointLightLocation.Color = glGetUniformLocation(this->m_program, "gPointLight.Base.Color");
	m_pointLightLocation.AmbientIntensity = glGetUniformLocation(this->m_program, "gPointLight.Base.AmbientIntensity");
	m_pointLightLocation.Position = glGetUniformLocation(this->m_program, "gPointLight.Position");
	m_pointLightLocation.DiffuseIntensity = glGetUniformLocation(this->m_program, "gPointLight.Base.DiffuseIntensity");
	m_pointLightLocation.Atten.Constant = glGetUniformLocation(this->m_program, "gPointLight.Atten.Constant");
	m_pointLightLocation.Atten.Linear = glGetUniformLocation(this->m_program, "gPointLight.Atten.Linear");
	m_pointLightLocation.Atten.Quadratic = glGetUniformLocation(this->m_program, "gPointLight.Atten.Quadratic");

	bool success = true;
	if (m_pointLightLocation.Color == INVALID_UNIFORM_LOCATION			  ||
		m_pointLightLocation.AmbientIntensity == INVALID_UNIFORM_LOCATION ||
		m_pointLightLocation.Position == INVALID_UNIFORM_LOCATION		  ||
		m_pointLightLocation.DiffuseIntensity == INVALID_UNIFORM_LOCATION ||
		m_pointLightLocation.Atten.Constant == INVALID_UNIFORM_LOCATION   ||
		m_pointLightLocation.Atten.Linear == INVALID_UNIFORM_LOCATION     ||
		m_pointLightLocation.Atten.Quadratic == INVALID_UNIFORM_LOCATION)
	{
		success = false;
	}
	
	return success;
}

void LightShader::SetPointLight(const PointLight& light)
{
	glUniform3f(m_pointLightLocation.Color, light.Color.x, light.Color.y, light.Color.z);
	glUniform1f(m_pointLightLocation.AmbientIntensity, light.AmbientIntensity);
	glUniform1f(m_pointLightLocation.DiffuseIntensity, light.DiffuseIntensity);
	glUniform3f(m_pointLightLocation.Position, light.Position.x, light.Position.y, light.Position.z);
	glUniform1f(m_pointLightLocation.Atten.Constant, light.Attenuation.Constant);
	glUniform1f(m_pointLightLocation.Atten.Linear, light.Attenuation.Linear);
	glUniform1f(m_pointLightLocation.Atten.Quadratic, light.Attenuation.Quadratic);
}

void LightShader::SetGPositionTextureUnit(unsigned int textureUnit) { glUniform1i(m_lightUniforms[G_POS_U], textureUnit); }
void LightShader::SetGDiffuseTextureUnit(unsigned int textureUnit){ glUniform1i(m_lightUniforms[G_DIF_U], textureUnit); }
void LightShader::SetGNormalTextureUnit(unsigned int textureUnit){ glUniform1i(m_lightUniforms[G_NOR_U], textureUnit);}
void LightShader::SetScreenSize(unsigned int width, unsigned int height) { glUniform2f(m_lightUniforms[G_SCREENSIZE_U], (float)width, (float)height); }
	
