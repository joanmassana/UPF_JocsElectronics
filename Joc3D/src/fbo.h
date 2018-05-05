#ifndef FBO_H
#define FBO_H

#include "includes.h"
#include "texture.h"

//FrameBufferObject
//helps rendering the scene inside a texture

class FBO {
public:
	GLuint fbo_id; 
	Texture* color_textures[4];
	Texture* depth_texture;

	GLuint renderbuffer_color;
	GLuint renderbuffer_depth;

	FBO();
	~FBO();

	bool create(int width, int height, int format, int num_textures = 1);
	bool createDepthOnly(int width, int height); //use this for shadowmaps
	
	void bind();
	void unbind();
};

#endif