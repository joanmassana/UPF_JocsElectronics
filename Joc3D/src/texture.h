/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com
	This contains a texture wrapper to use textures. It allow to load TGAs.
*/
#ifndef TEXTURE_H
#define TEXTURE_H

#include "includes.h"
#include <map>
#include <string>

class Shader;

// TEXTURE CLASS
class Texture
{
public:
	//a general struct to store all the information about a TGA file
	typedef struct ImageInfo 
	{
		unsigned int width;
		unsigned int height;
		unsigned int bpp; //bits per pixel
		bool origin_topleft;
		Uint8* data; //bytes with the pixel information
		bool BGR; //colors encoded as BGR instead of RGB (TGA work like this)
	} TGAInfo;

	//textures manager
	static std::map<std::string, Texture*> sTexturesLoaded;

	GLuint texture_id; // GL id to identify the texture in opengl, every texture must have its own id
	float width;
	float height;
	std::string filename;

	unsigned int format;
	unsigned int type;
	unsigned int texture_type; //GL_TEXTURE_2D, GL_TEXTURE_CUBE
	bool mipmaps;

	//original data info
	ImageInfo info;

	Texture();
	Texture(unsigned int width, unsigned int height, unsigned int format = GL_RGB, unsigned int type = GL_UNSIGNED_BYTE, bool mipmaps = true, Uint8* data = NULL, unsigned int data_format = 0);
	~Texture();

	void create(unsigned int width, unsigned int height, unsigned int format = GL_RGB, unsigned int type = GL_UNSIGNED_BYTE, bool mipmaps = true, Uint8* data = NULL, unsigned int data_format = 0);

	void bind();
	void unbind();
	static void UnbindAll();

	//load without using the manager
	bool load(const char* filename, bool mipmaps = true, bool wrap = true, bool upload_to_vram = true);

	//load using the manager (caching loaded ones to avoid reloading them)
	static Texture* Load(const char* filename, bool mipmaps = true, bool wrap = true, bool upload_to_vram = true);

	void generateMipmaps();

	//show the texture on the current viewport
	void toViewport( Shader* shader = NULL );

protected:
	ImageInfo* loadTGA(const char* filename);
	ImageInfo* loadPNG(const char* filename);
};

bool isPowerOfTwo(int n);

#endif