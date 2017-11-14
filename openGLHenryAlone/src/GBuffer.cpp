#include "GBuffer.h"
#include <iostream>


GBuffer::GBuffer(unsigned int WindowWidth, unsigned int WindowHeight)
{
	this->Init(WindowWidth, WindowHeight);
}


GBuffer::~GBuffer()
{
}

bool GBuffer::Init(unsigned int WindowWidth, unsigned int WindowHeight)
{
	glGenFramebuffers(1, &m_frameBufferObject);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->m_frameBufferObject);

	// create gbuffer textures
	glGenTextures(GBUFFER_NUM_TEXTURES, this->m_textures);
	glGenTextures(1, &m_depthTexture);

	for (size_t i = 0; i < GBUFFER_NUM_TEXTURES; i++)
	{
		glBindTexture(GL_TEXTURE_2D, this->m_textures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, 0); // last 0 could be NULL
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, this->m_textures[i], 0);
	}

	// depth is special
	glBindTexture(GL_TEXTURE_2D, this->m_depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0); // last 0 could be NULL
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->m_depthTexture, 0);

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };

	glDrawBuffers(GBUFFER_NUM_TEXTURES, drawBuffers);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Framebuffer error, status: " << status << std::endl;
		return false;
	}

	// restore
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	return true;
}

void GBuffer::BindForWriting()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->m_frameBufferObject);
}

void GBuffer::BindForReading()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	for (size_t i = 0 ; i < GBUFFER_NUM_TEXTURES; i++) {
        glActiveTexture(GL_TEXTURE0 + i);	
        glBindTexture(GL_TEXTURE_2D, m_textures[GBUFFER_TEXTURE_TYPE_POSITION + i]);
    }
}

void GBuffer::SetReadBuffer(GBUFFER_TEXTURE_TYPE textureType) 
{
	glReadBuffer(GL_COLOR_ATTACHMENT0 + textureType);
}