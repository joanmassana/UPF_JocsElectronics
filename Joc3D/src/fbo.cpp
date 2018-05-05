#include "fbo.h"
#include <cassert>
FBO::FBO()
{
	fbo_id = 0;
	color_textures[0] = color_textures[1] = color_textures[2] = color_textures[3] = NULL;
	depth_texture = NULL;

	renderbuffer_color = 0;
	renderbuffer_depth = 0;
}

FBO::~FBO()
{
	for (int i = 0; i < 4; ++i)
	{
		Texture* t = color_textures[i];
		if (!t)
			continue;
		delete t;
	}
	if(fbo_id)
		glDeleteFramebuffers(1, &fbo_id);
	if (renderbuffer_color)
		glDeleteRenderbuffersEXT(1, &renderbuffer_color);
	if (renderbuffer_depth)
		glDeleteRenderbuffersEXT(1, &renderbuffer_depth);
}

bool FBO::create( int width, int height, int format, int num_textures )
{
	assert(glGetError() == GL_NO_ERROR);

	//create textures
	for (int i = 0; i < num_textures; ++i)
		color_textures[i] = new Texture(width, height, GL_RGBA, GL_UNSIGNED_BYTE, false );
	depth_texture = new Texture(width, height, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, false);

	glGenFramebuffersEXT(1, &fbo_id);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_id);

	for (int i = 0; i < num_textures; ++i)
	{
		Texture* color_texture = color_textures[i];
		glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i, GL_TEXTURE_2D, color_texture->texture_id, 0);
	}
	glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture->texture_id, 0 );

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
	{
		std::cout << "Error: Framebuffer object is not completed: " << status  << std::endl;
		return false;
	}
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	assert(glGetError() == GL_NO_ERROR);
	return true;
}

bool FBO::createDepthOnly(int width, int height)
{
	glGenFramebuffersEXT(1, &fbo_id);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_id);

	glGenRenderbuffersEXT(1, &renderbuffer_color);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, renderbuffer_color);

	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_RGBA, width, height);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER_EXT, renderbuffer_color);

	depth_texture = new Texture(width, height, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, false);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture->texture_id, 0);

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
	{
		std::cout << "Error: Framebuffer object is not completed" << std::endl;
		return false;
	}
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	return true;
}

void FBO::bind()
{
	assert(glGetError() == GL_NO_ERROR);
	Texture* tex = color_textures[0] ? color_textures[0] : depth_texture;
	assert(tex && "framebuffer without texture");
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_id);
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, (int)tex->width, (int)tex->height);
}

void FBO::unbind()
{
	// output goes to the FBO and it’s attached buffers
	glPopAttrib();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	assert(glGetError() == GL_NO_ERROR);
}