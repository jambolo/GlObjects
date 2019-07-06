/** @file *//********************************************************************************************************

                                                      SkyBox.cpp

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/GlObjects/SkyBox/SkyBox.cpp#4 $

	$NoKeywords: $

 ********************************************************************************************************************/

#include "PrecompiledHeaders.h"

#include "SkyBox.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <gl/gl.h>

#include "GlObjects/TextureLoader/TextureLoader.h"
#include "Glx/Glx.h"
#include "TgaFile/TgaFile.h"
#include "Misc/SafeStr.h"


namespace GlObjects
{


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

/// @param	sFilename	Base file name of the TGA files containing the skybox textures. The actual files names are
///						created by appending "_-Z.tga", "_+Z.tga", "_+X.tga", "_-X.tga", "_+Y.tga", "_-Y.tga" to
///						the base name. Only textures for faces that are to be rendered (specified by @p facemask)
///						are needed.
/// @param	faceMask	Which faces of the skybox are to be drawn. This value is set by ORing the appropriate Faces
///						values together.
/// @warning			This function may throw a <tt>std::runtime_error</tt>, or a <tt>std::bad_alloc</tt>.

SkyBox::SkyBox( char const * fileName, unsigned faceMask/* = FACE_ALL_FACES*/ )
{
	assert( Glx::Extension::IsSupported( "GL_EXT_bgra" ) );

	for ( int face = 0; face < NUM_FACES; face++ )
	{
		if ( ( faceMask & ( 1 << face ) ) != 0 )
		{
			static GlObjects::TextureLoader	textureLoader;
			static char const				aSuffixes[ NUM_FACES ][4] =
											{
												"_-Z",
												"_+Z",
												"_+X",
												"_-X",
												"_+Y",
												"_-Y"
											};

			// Create the full name of the file

			char path[ _MAX_PATH ];

			SafeStrcpy( path, fileName, sizeof( path ) );
			SafeStrcat( path, aSuffixes[ face ], sizeof( path ) );
			SafeStrcat( path, ".tga", sizeof( path ) );

			m_aTextures[ face ] = textureLoader.Load( path, GL_CLAMP ).release();
		}
		else
		{
			m_aTextures[ face ] = 0;
		}
	}
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

SkyBox::~SkyBox()
{
	for ( int face = 0; face < NUM_FACES; face++ )
	{
		delete m_aTextures[ face ];
	}
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

/// @param	camera	The camera for the scene
/// @param	bTestZ	If @c true, then enable depth-testing when drawing
///
/// @note	This function sets the following states:
///				- glEnable( GL_DEPTH_TEST ), if @a bTestZ is @c true
///				- glDisable( GL_DEPTH_TEST ), if @a bTestZ is @c false
///				- glDepthMask( GL_FALSE )
///				- glEnable( GL_TEXTURE_2D )
///				- glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE )
///
///
/// @note	Depth-testing is necessary only if you do not render the skybox first. Obviously, if depth-testing is
///			enabled, the depth-buffer must be initialized beforehand.


void SkyBox::Apply( Glx::Camera const & camera, bool bTestZ/* = false*/ )
{
	// Draw the skybox using this camera. Center the sides between the near and far clip planes (to make sure they
	// aren't clipped.

	Apply( camera.GetPosition(),
		   ( camera.GetNearDistance() + camera.GetFarDistance() ) / ( 1.f + float( Math::SQRT_OF_3 ) ),
		   bTestZ );
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

/// @param	vp	Location of the view point (the skybox is centered at this point)
/// @param	r	Distance from the center of the skybox to a side.
/// @param	bTestZ	If @c true, then enable depth-testing when rendering
///
/// @note	To ensure that the sides and corners of the skybox are not clipped by the near and far planes, use this
///			value for @a r: <tt>r = ( n + f ) / ( 1 + sqrt(3) )</tt>. The skybox will be not be clipped as long as
///			<tt>f >= n * sqrt(3)</tt>
///
/// @note	This function sets the following states:
///				- glEnable( GL_DEPTH_TEST ), if @a bTestZ is @c true
///				- glDisable( GL_DEPTH_TEST ), if @a bTestZ is @c false
///				- glDepthMask( GL_FALSE )
///				- glEnable( GL_TEXTURE_2D )
///				- glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE )
///
/// @note	Depth-testing is necessary only if you do not render the skybox first. Obviously, if depth-testing is
///			enabled, the depth-buffer must be initialized beforehand.

void SkyBox::Apply( Vector3 const & vp, float r, bool bTestZ/* = false*/ )
{
	glPushMatrix();

	if ( bTestZ )
	{
		Glx::Enable( GL_DEPTH_TEST );
	}
	else
	{
		Glx::Disable( GL_DEPTH_TEST );
	}
	glDepthMask( GL_FALSE );

	// The skybox is drawn as a scaled and tranlated unit cube

	glTranslatef( vp.m_X,  vp.m_Y, vp.m_Z );
	glScalef( r, r, r );

	Glx::Enable( GL_TEXTURE_2D );
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

	static GLfloat const aTexCoords[ NUM_FACES ][ 4 ][ 2 ] =
	{
		{	{ 0.f, 0.f }, { 1.f, 0.f }, { 1.f, 1.f }, { 0.f, 1.f }	},	// -Z
		{	{ 0.f, 0.f }, { 1.f, 0.f }, { 1.f, 1.f }, { 0.f, 1.f }	},	// +Z
		{	{ 0.f, 0.f }, { 1.f, 0.f }, { 1.f, 1.f }, { 0.f, 1.f }	},	// +X
		{	{ 0.f, 0.f }, { 1.f, 0.f }, { 1.f, 1.f }, { 0.f, 1.f }	},	// -X
		{	{ 0.f, 0.f }, { 1.f, 0.f }, { 1.f, 1.f }, { 0.f, 1.f }	},	// +Y
		{	{ 0.f, 0.f }, { 1.f, 0.f }, { 1.f, 1.f }, { 0.f, 1.f }	}	// -Y
	};

	static GLfloat const aVertices[ NUM_FACES ][ 4 ][ 3 ] =
	{
		{	{ -1.f, -1.f, -1.f }, {  1.f, -1.f, -1.f }, {  1.f,  1.f, -1.f }, { -1.f,  1.f, -1.f }	},	// -Z
		{	{ -1.f,  1.f,  1.f }, {  1.f,  1.f,  1.f }, {  1.f, -1.f,  1.f }, { -1.f, -1.f,  1.f }	},	// +Z
		{	{  1.f,  1.f, -1.f }, {  1.f, -1.f, -1.f }, {  1.f, -1.f,  1.f }, {  1.f,  1.f,  1.f }	},	// +X
		{	{ -1.f, -1.f, -1.f }, { -1.f,  1.f, -1.f }, { -1.f,  1.f,  1.f }, { -1.f, -1.f,  1.f }	},	// -X
		{	{ -1.f,  1.f, -1.f }, {  1.f,  1.f, -1.f }, {  1.f,  1.f,  1.f }, { -1.f,  1.f,  1.f }	},	// +Y
		{	{  1.f, -1.f, -1.f }, { -1.f, -1.f, -1.f }, { -1.f, -1.f,  1.f }, {  1.f, -1.f,  1.f }	},	// -Y
	};

	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glEnableClientState( GL_VERTEX_ARRAY );
	glTexCoordPointer( 2, GL_FLOAT, 2 * sizeof( GLfloat ), aTexCoords );
	glVertexPointer( 3, GL_FLOAT, 3 * sizeof( GLfloat ), aVertices );

	for ( int face = 0; face < NUM_FACES; face++ )
	{
		if ( m_aTextures[ face ] )
		{
			m_aTextures[ face ]->Apply();

			glDrawArrays( GL_QUADS, face * 4, 4 );
		}
	}

	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );

	glPopMatrix();
}


} // namespace GlObjects
