// COMP710 GP Framework 2025

// This include:
#include "texture.h"

// Local includes:
#include "logmanager.h"

// Library include:
#include <SDL_image.h>
#include <cassert>
#include "glew.h"
#include <iostream>

Texture::Texture()
	: m_uiTextureId(0)
	, m_iHeight(0)
	, m_iWidth(0)
{

}

Texture::~Texture()
{
	if (m_uiTextureId != 0) {
		glDeleteTextures(1, &m_uiTextureId);
	}
}

static void FlipSurfaceVertically(SDL_Surface* surf)
{
	if (!surf || !surf->pixels) return;

	int pitch = surf->pitch;            // bytes per row
	auto* temp = new uint8_t[pitch];    // one row buffer
	auto* pixels = static_cast<uint8_t*>(surf->pixels);

	for (int y = 0, y2 = surf->h - 1; y < y2; ++y, --y2)
	{
		uint8_t* row = pixels + y * pitch;
		uint8_t* rowOpp = pixels + y2 * pitch;

		// swap rows
		memcpy(temp, row, pitch);
		memcpy(row, rowOpp, pitch);
		memcpy(rowOpp, temp, pitch);
	}

	delete[] temp;
}

bool Texture::Initialise(const char* pcFilename)
{	SDL_Surface* pSurface = IMG_Load(pcFilename);

	if (pSurface)
	{
		m_iWidth = pSurface->w;
		m_iHeight = pSurface->h;

		int bytesPerPixel = pSurface->format->BytesPerPixel;

		unsigned int format = 0;

		FlipSurfaceVertically(pSurface);

		if (bytesPerPixel == 3)
		{
			format = GL_RGB;
		}
		else if (bytesPerPixel == 4)
		{
			format = GL_RGBA;
		}

		glGenTextures(1, &m_uiTextureId);
		glBindTexture(GL_TEXTURE_2D, m_uiTextureId);

		glTexImage2D(GL_TEXTURE_2D, 0, format, m_iWidth, m_iHeight, 0,
		             format, GL_UNSIGNED_BYTE, pSurface->pixels);
		
		SDL_FreeSurface(pSurface);
		pSurface = 0;
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	
	else
	{
		LogManager::GetInstance().Log("Texture failed to load!");
		assert(0);
		return false;
	}
	
	return true;
}

void Texture::SetActive()
{
	glBindTexture(GL_TEXTURE_2D, m_uiTextureId);
}

int Texture::GetWidth() const
{
	assert(m_iWidth);
	return (m_iWidth);
}

int Texture::GetHeight() const
{
	assert(m_iHeight);
	return (m_iHeight);
}

void
Texture::LoadTextTexture(const char* text, const char* fontname, int pointsize)
{
	static bool ttfInitialised = false;

	TTF_Font* pFont = 0;
	
	if (!ttfInitialised) {
		TTF_Init();
		ttfInitialised = true;
	}

	if (pFont == 0)
	{
		pFont = TTF_OpenFont(fontname, pointsize);
	}
	
	SDL_Color color;
	color.r = 255;
	color.g = 255;
	color.b = 255;
	color.a = 255;
	
	SDL_Surface* pSurface = TTF_RenderText_Blended(pFont, text, color);
	
	// Convert the surface to a consistent 32-bit RGBA format. (Text is garbled without this)
	SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat(pSurface, SDL_PIXELFORMAT_RGBA32, 0);
	SDL_FreeSurface(pSurface);  // Free the original surface
	pSurface = formattedSurface;

	FlipSurfaceVertically(pSurface);
	LoadSurfaceIntoTexture(pSurface);
	
	TTF_CloseFont(pFont);
	pFont = 0;
}

void
Texture::LoadSurfaceIntoTexture(SDL_Surface* pSurface)
{
	if (pSurface)
	{
		m_iWidth = pSurface->w;
		m_iHeight = pSurface->h;
		int bytesPerPixel = pSurface->format->BytesPerPixel;
		unsigned int format = 0;
		
		if (bytesPerPixel == 3)
		{
			format = GL_RGB;
		}
		else if (bytesPerPixel == 4)
		{
			format = GL_RGBA;
		}
		
		glGenTextures(1, &m_uiTextureId);
		glBindTexture(GL_TEXTURE_2D, m_uiTextureId);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexImage2D(GL_TEXTURE_2D, 0, format, m_iWidth, m_iHeight, 0, format,
				  	GL_UNSIGNED_BYTE, pSurface->pixels);
		SDL_FreeSurface(pSurface);
		pSurface = 0;
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
}

void Texture::SetID(GLuint id)
{
	m_uiTextureId = id;
}