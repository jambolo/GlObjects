/** @file *//********************************************************************************************************

                                                  TextureLoader.cpp

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/GlObjects/TextureLoader/TextureLoader.cpp#4 $

	$NoKeywords: $

 ********************************************************************************************************************/

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include "TextureLoader.h"

#include "Glx/Texture.h"
#include "Glx/MipMappedTexture.h"
#include "Misc/auto_array_ptr.h"
#include "Misc/Types.h"
#include "TgaFile/TgaFile.h"

#include <cassert>
#include <memory>
#include <utility>

namespace GlObjects
{


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

/// This function loads a texture from a TGA file. The file format must be uncompressed 24-bit or 32-bit.
///
/// @param	sFileName		Name of the file to load
/// @param	wrap			Wrap mode. The value can be any valid value for the @c GL_TEXTURE_WRAP_S texture parameter.
/// @param	minFiltering	Filtering mode used when texels are "minified". The value can be any valid value for the
///							@c GL_TEXTURE_MIN_FILTER texture parameter.
/// @param	magFiltering	Filtering mode used when texels are "magnified". The value can be any valid value for the
///							@c GL_TEXTURE_MAG_FILTER texture parameter.
/// @param	id				Texture id. If 0 is specified, then an id is
///
/// @return		An @c std::auto_ptr to the loaded texture.
///
/// @warning	This function may throw a ConstructorFailedException, <tt>std::runtime_error</tt>, or a
///				<tt>std::bad_alloc</tt>.

std::auto_ptr< Glx::Texture > TextureLoader::Load( char const * sFileName,
												   GLenum wrap				/* = GL_REPEAT*/,
												   GLenum minFiltering		/* = GL_LINEAR*/,
												   GLenum magFiltering		/* = GL_LINEAR*/,
												   GLuint id				/* = 0*/ )
{
	Glx::Texture *	pTexture	= 0;

	try
	{
		bool					ok;
		TgaFile					file( sFileName );
		auto_array_ptr< uint8 >	qaImageData;
		GLenum					format;
		int						texelSize;
		
		// Determine the characteristics of this image

		if ( file.m_ImageType == TgaFile::IMAGE_TRUECOLOR )
		{
			if ( file.m_Depth == 32 && file.m_AlphaDepth == 8 )
			{
				texelSize	= 4;
				format		= GL_BGRA_EXT;
			}
			else if ( file.m_Depth == 24 && file.m_AlphaDepth == 0 )
			{
				texelSize	= 3;
				format		= GL_BGR_EXT;
			}
			else
			{
				throw std::runtime_error( "Invalid pixel format" );
			}
		}
		else
		{
			throw std::runtime_error( "Invalid pixel format" );
		}

		// Create the texture

		pTexture = new Glx::Texture( file.m_Width, file.m_Height,
									 qaImageData.get(),
									 format, GL_UNSIGNED_BYTE, wrap, minFiltering, magFiltering,
									 id );
		if ( pTexture == 0 ) throw std::bad_alloc();

		// Allocate temporary space for the image data

		qaImageData = auto_array_ptr< uint8 >( new uint8[ file.m_Width * file.m_Height * texelSize ] );
		if ( qaImageData.get() == 0 ) throw std::bad_alloc();

		// Load the image data

		ok = file.Read( qaImageData.get(), TgaFile::ORDER_BOTTOMLEFT );
		if ( !ok ) throw std::runtime_error("TGAFile read failed" );
	}
	catch ( ... )
	{
		delete pTexture;
		pTexture = 0;
	}

	return std::auto_ptr< Glx::Texture >( pTexture );
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

/// This function loads a mip-mapped texture from a TGA file. The file format must be uncompressed 24-bit or 32-bit.
///
/// @param	sFileName		Name of the file to load
/// @param	wrap			Wrap mode. The value can be any valid value for the @c GL_TEXTURE_WRAP_S texture parameter.
/// @param	minFiltering	Filtering mode used when texels are "minified". The value can be any valid value for the
///							@c GL_TEXTURE_MIN_FILTER texture parameter.
/// @param	magFiltering	Filtering mode used when texels are "magnified". The value can be any valid value for the
///							@c GL_TEXTURE_MAG_FILTER texture parameter.
/// @param	id				Texture id. If 0 is specified, then an id is
///
/// @return		An @c std::auto_ptr to the loaded texture.
///
/// @warning	This function may throw a ConstructorFailedException, <tt>std::runtime_error</tt>, or a
///				<tt>std::bad_alloc</tt>.

std::auto_ptr< Glx::MipMappedTexture > TextureLoader::LoadMipMapped( char const * sFileName,
												   					 GLenum wrap			/* = GL_REPEAT*/,
												   					 GLenum minFiltering	/* = GL_LINEAR_MIPMAP_LINEAR*/,
												   					 GLenum magFiltering	/* = GL_LINEAR*/,
												   					 GLuint id				/* = 0*/ )
{
	Glx::MipMappedTexture *	pTexture	= 0;

	try
	{
		bool					ok;
		TgaFile					file( sFileName );
		auto_array_ptr< uint8 >	qaImageData;
		GLenum					format;
		int						texelSize;
		
		// Determine the characteristics of this image

		if ( file.m_ImageType == TgaFile::IMAGE_TRUECOLOR )
		{
			if ( file.m_Depth == 32 && file.m_AlphaDepth == 8 )
			{
				texelSize	= 4;
				format		= GL_BGRA_EXT;
			}
			else if ( file.m_Depth == 24 && file.m_AlphaDepth == 0 )
			{
				texelSize	= 3;
				format		= GL_BGR_EXT;
			}
			else
			{
				throw std::runtime_error( "Invalid pixel format" );
			}
		}
		else
		{
			throw std::runtime_error( "Invalid pixel format" );
		}

		// Create the texture

		pTexture = new Glx::MipMappedTexture( file.m_Width, file.m_Height,
											  format, GL_UNSIGNED_BYTE,
											  wrap, minFiltering, magFiltering,
											  id );
		if ( pTexture == 0 ) throw std::bad_alloc();

		// Allocate temporary space for the image data

		qaImageData = auto_array_ptr< uint8 >( new uint8[ file.m_Width * file.m_Height * texelSize ] );
		if ( qaImageData.get() == 0 ) throw std::bad_alloc();

		// Load the image data

		ok = file.Read( qaImageData.get(), TgaFile::ORDER_BOTTOMLEFT );
		if ( !ok ) throw std::runtime_error( "TGAFile read failed" );

		pTexture->BuildAllMipMaps( qaImageData.get() );
	}
	catch ( ... )
	{
		delete pTexture;
		pTexture = 0;
	}

	return std::auto_ptr< Glx::MipMappedTexture >( pTexture );
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

/// This function loads a mip-mapped texture from a TGA file. The file format must be uncompressed 24-bit or 32-bit.
///
/// @param	pasFileNames	Array of the names of the files to load
/// @param	nLevels			Number of names in the array
/// @param	wrap			Wrap mode. The value can be any valid value for the @c GL_TEXTURE_WRAP_S texture parameter.
/// @param	minFiltering	Filtering mode used when texels are "minified". The value can be any valid value for the
///							@c GL_TEXTURE_MIN_FILTER texture parameter.
/// @param	magFiltering	Filtering mode used when texels are "magnified". The value can be any valid value for the
///							@c GL_TEXTURE_MAG_FILTER texture parameter.
/// @param	id				Texture id. If 0 is specified, then an id is
///
/// @return		An @c std::auto_ptr to the loaded texture.
///
/// @warning	This function may throw a ConstructorFailedException, <tt>std::runtime_error</tt>, or a
///				<tt>std::bad_alloc</tt>.

std::auto_ptr< Glx::MipMappedTexture > TextureLoader::LoadMipMapped( char const * const * pasFileNames,
																	 int nLevels,
												   					 GLenum wrap			/* = GL_REPEAT*/,
												   					 GLenum minFiltering	/* = GL_LINEAR_MIPMAP_LINEAR*/,
												   					 GLenum magFiltering	/* = GL_LINEAR*/,
												   					 GLuint id				/* = 0*/ )
{
	assert( nLevels > 0 );

	Glx::MipMappedTexture *	pTexture	= 0;

	// Load the highest resolution mip level first to get the size and format of the texture

	try
	{
		TgaFile		file( pasFileNames[ 0 ] );
		GLenum		format;
		int			texelSize;
		int const	pixelDepth	= file.m_Depth;
		int const	alphaDepth	= file.m_AlphaDepth;
		
		// Determine the characteristics of this image

		if ( file.m_ImageType == TgaFile::IMAGE_TRUECOLOR )
		{
			if ( pixelDepth == 32 && alphaDepth == 8 )
			{
				texelSize	= 4;
				format		= GL_BGRA_EXT;
			}
			else if ( pixelDepth == 24 && alphaDepth == 0 )
			{
				texelSize	= 3;
				format		= GL_BGR_EXT;
			}
			else
			{
				throw std::runtime_error( "Invalid pixel format" );
			}
		}
		else
		{
			throw std::runtime_error( "Invalid pixel format" );
		}

		// Create the texture

		pTexture = new Glx::MipMappedTexture( file.m_Width, file.m_Height,
											  format, GL_UNSIGNED_BYTE,
											  wrap, minFiltering, magFiltering,
											  id );
		if ( pTexture == 0 ) throw std::bad_alloc();
		
		// Load the first mip level

		{
			bool	ok;

			auto_array_ptr< uint8 >	qaImageData( new uint8[ file.m_Width * file.m_Height * texelSize ] );
			if ( qaImageData.get() == 0 ) throw std::bad_alloc();

			// Load the image data

			ok = file.Read( qaImageData.get(), TgaFile::ORDER_BOTTOMLEFT );
			if ( !ok ) throw std::runtime_error( "TGAFile read failed" );

			pTexture->AddMipMap( 0, qaImageData.get() );
		}

		// Load the rest of the mip levels

		for ( int i = 1; i < nLevels; i++ )
		{
			TgaFile					file( pasFileNames[i] );
			auto_array_ptr< uint8 >	qaImageData;
			bool					ok;
	
			// Determine the characteristics of this image. Make sure they match the first mip level.

			if ( file.m_ImageType != TgaFile::IMAGE_TRUECOLOR ||
				 file.m_Depth != pixelDepth ||
				 file.m_AlphaDepth != alphaDepth )
			{
				throw std::runtime_error( "Inconsistent pixel format" );
			}

			// Allocate temporary space for the image data

			qaImageData = auto_array_ptr< uint8 >( new uint8[ file.m_Width * file.m_Height * texelSize ] );
			if ( qaImageData.get() == 0 ) throw std::bad_alloc();

			// Load the image data

			ok = file.Read( qaImageData.get(), TgaFile::ORDER_BOTTOMLEFT );
			if ( !ok ) throw std::runtime_error( "TGAFile read failed" );

			pTexture->AddMipMap( i, qaImageData.get() );
		}
	}
	catch ( ... )
	{
		delete pTexture;
		pTexture = 0;
	}

	return std::auto_ptr< Glx::MipMappedTexture >( pTexture );
}


} // namespace GlObjects




