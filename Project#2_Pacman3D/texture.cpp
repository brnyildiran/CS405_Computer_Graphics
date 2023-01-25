#include <iostream>
#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint loadTexture(const char *textureFilePath) {
	stbi_set_flip_vertically_on_load(1);
	int width, height, num_components;
	unsigned char *imageData = stbi_load(textureFilePath, &width, &height, &num_components, 0);
	unsigned int textureId;
	glGenTextures(1, &textureId);
	if (imageData) {
		GLenum format{};
		if (num_components == 1) {
			format = GL_RED;
		} else if (num_components == 3) {	
			format = GL_RGB;
		}	else if (num_components == 4)	{	
			format = GL_RGBA;
		}
		glBindTexture(GL_TEXTURE_2D, textureId);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, imageData);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		stbi_image_free(imageData);
		std::cout << "Image loaded successfully" << std::endl;
	} else {
		stbi_image_free(imageData);
		std::cout << "Image failed to load" << std::endl;
	}
	return textureId;
}

#define FOURCC_DXT1 0x31545844
#define FOURCC_DXT3 0x33545844
#define FOURCC_DXT5 0x35545844

GLuint loadDDS(const char *imagepath) {
	unsigned char header[124];
	FILE *fp = fopen(imagepath, "rb");
	if (fp == NULL) {
		std::cout << "Image failed to load - 1" << std::endl;
		return 0;
	}
	char filecode[4];
	fread(filecode, 1, 4, fp);
	if (strncmp(filecode, "DDS ", 4) != 0) {
		fclose(fp);
		std::cout << "Image failed to load - 2" << std::endl;
		return 0;
	}
	fread(&header, 124, 1, fp);
	unsigned int height = *(unsigned int *)&(header[8]);
	unsigned int width = *(unsigned int *)&(header[12]);
	unsigned int linearSize = *(unsigned int *)&(header[16]);
	unsigned int mipMapCount = *(unsigned int *)&(header[24]);
	unsigned int fourCC = *(unsigned int *)&(header[80]);
	unsigned char *buffer;
	unsigned int bufsize;
	bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
	buffer = (unsigned char *)malloc(bufsize * sizeof(unsigned char));
	fread(buffer, 1, bufsize, fp);
	fclose(fp);
	unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
	unsigned int format;
	switch (fourCC) {
	case FOURCC_DXT1:
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		break;
	case FOURCC_DXT3:
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		break;
	case FOURCC_DXT5:
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		break;
	default:
		free(buffer);
		std::cout << "Image failed to load - 3" << std::endl;
		return 0;
	}
	GLuint textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
	unsigned int offset = 0;
	for (unsigned int level = 0; level < mipMapCount && (width || height); level++) {
		unsigned int size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,	0, size, buffer + offset);
		offset += size;
		width /= 2;
		height /= 2;
		if (width < 1) { width = 1; }
		if (height < 1) { height = 1; }
	}
	free(buffer);
	std::cout << "Image loaded successfully" << std::endl;
	return textureId;
}