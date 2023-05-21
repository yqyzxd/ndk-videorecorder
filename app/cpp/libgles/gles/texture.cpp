#include "./texture.h"
#include "../../utils/log.h"

#define LOG_TAG "Texture"

Texture::Texture(GLenum target) {
	this->target=target;
}

Texture::~Texture() {
}

bool Texture::createTexture() {

	textureId = 0;
	int ret = initTexture();
	LOGE("createTexture:%d",textureId);
	checkGlError("createTexture");
	if (ret < 0) {
		this->dealloc();
		return false;
	}
	return true;
}

void Texture::updateTexImage(void* pixels, int frameWidth, int frameHeight) {
	if (pixels) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(target, textureId);
		if (checkGlError("glBindTexture")) {
			return;
		}
		glTexImage2D(target, 0, GL_RGBA, frameWidth, frameHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	}
}

bool Texture::bindTexture(GLint uniformSampler) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, textureId);
	if (checkGlError("glBindTexture")) {
		return false;
	}
	glUniform1i(uniformSampler, 0);
	return true;
}

int Texture::initTexture() {
	glGenTextures(1, &textureId);
	glBindTexture(target, textureId);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(target, 0);
	return 1;
}

GLuint Texture::getTextureId() {
	return textureId;
}

bool Texture::checkGlError(const char* op) {
	GLint error;
	for (error = glGetError(); error; error = glGetError()) {
		LOGE( "[Texture] after %s() glError (0x%x)\n", op, error);
		return true;
	}
	return false;
}

void Texture::dealloc() {
	if (textureId) {
		glDeleteTextures(1, &textureId);
	}
}
