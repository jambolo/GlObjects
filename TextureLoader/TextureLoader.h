#if !defined( GLOBJECTS_TEXTURELOADER_H_INCLUDED )
#define GLOBJECTS_TEXTURELOADER_H_INCLUDED

#pragma once

/** @file *//********************************************************************************************************

                                                   TextureLoader.h

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/GlObjects/TextureLoader/TextureLoader.h#2 $

	$NoKeywords: $

 ********************************************************************************************************************/

#include <windows.h>
#include <memory>
#include <gl/gl.h>
#include "Glx/Texture.h"
#include "Glx/MipMappedTexture.h"

namespace GlObjects
{


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

/// A class that loads a TGA file into a texture

class TextureLoader
{
public:
	/// Loads a texture
	static std::auto_ptr< Glx::Texture > Load( char const * sFileName,
											   GLenum wrap				= GL_REPEAT,
											   GLenum minFiltering		= GL_LINEAR,
											   GLenum magFiltering		= GL_LINEAR,
											   GLuint id				= 0 );

	/// Loads a mip-mapped texture
	static std::auto_ptr< Glx::MipMappedTexture > LoadMipMapped( char const * sFileName,
																 GLenum wrap			= GL_REPEAT,
																 GLenum minFiltering	= GL_LINEAR_MIPMAP_LINEAR,
																 GLenum magFiltering	= GL_LINEAR,
																 GLuint id				= 0 );

	/// Loads a mip-mapped texture with custom mip levels
	static std::auto_ptr< Glx::MipMappedTexture > LoadMipMapped( char const * const * pasFileNames,
																 int nLevels,
																 GLenum wrap			= GL_REPEAT,
																 GLenum minFiltering	= GL_LINEAR_MIPMAP_LINEAR,
																 GLenum magFiltering	= GL_LINEAR,
																 GLuint id				= 0 );
};


} // namespace GlObjects


#endif // !defined( GLOBJECTS_TEXTURELOADER_H_INCLUDED )
