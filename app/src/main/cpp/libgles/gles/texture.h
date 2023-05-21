#ifndef PIC_PREVIEW_TEXTURE_H
#define PIC_PREVIEW_TEXTURE_H
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>


class Texture {
private:
	GLuint textureId;
	GLenum target;//GL_TEXTURE_2D or GL_TEXTURE_EXTERNAL_OES
	int initTexture();

	bool checkGlError(const char* op);
public:
	Texture(GLenum target);
	Texture(): Texture(GL_TEXTURE_2D){};

	virtual ~Texture();
	GLuint getTextureId();
	bool createTexture();
	void updateTexImage(void* pixels, int width, int height);
	bool bindTexture(GLint uniformSampler);
	void dealloc();
};

#endif //PIC_PREVIEW_TEXTURE_H
