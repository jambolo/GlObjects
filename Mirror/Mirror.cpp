/** @file *//********************************************************************************************************

                                                      Mirror.cpp

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/GlObjects/Mirror/Mirror.cpp#1 $

	$NoKeywords: $

 ********************************************************************************************************************/

#include "Mirror.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <gl/gl.h>

#include "Glx/Camera.h"
#include "Glx/Enable.h"
#include "Math/Plane.h"
#include "Math/Matrix44.h"
#include "Math/Matrix33.h"
#include "Math/Point.h"

#include <algorithm>


namespace GlObjects
{


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

/// @param	position		Location of the center of the mirror
/// @param	orientation		Orientation of the mirror. The normal of an unrotated mirror is (0,0,1).
/// @param	w,h				Size of the mirror in world units
/// @param	tw,th			Size of the texture in texels
///
/// @warn	This function may throw <tt>std::bad_alloc</tt>.

Mirror::Mirror( Vector3 const & position, Quaternion const & orientation, float w, float h, int tw, int th )
	:m_Frame( position, orientation, Vector3( 1.0f, 1.0f, 1.0f ) ),
	m_MirrorWidth( w ),
	m_MirrorHeight( h ),
	m_IsReflecting( false )
{
	m_pTexture = new Glx::Texture( tw, th, GL_CLAMP );
	if ( m_pTexture == 0 ) throw std::bad_alloc();
	
	// Set its size, format, and border by creating a texture with dummy data

	glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 0, 0, tw, th, 0 );

	m_pMaterial = new Glx::Material( m_pTexture, GL_REPLACE, Glx::Rgba::WHITE, Glx::Rgba::BLACK, 0.f, Glx::Rgba::BLACK, GL_FLAT );
	if ( m_pMaterial == 0 ) throw std::bad_alloc();
}




///********************************************************************************************************************/
///*																													*/
///*																													*/
///********************************************************************************************************************/
//
//Mirror::Mirror( Glx::Frame const & frame, int tw, int th )
//	: m_Frame( frame ),
//	m_MirrorWidth( width ),
//	m_MirrorHeight( height ),
//	m_TextureWidth( tw ),
//	m_TextureHeight( th ),
//	m_TextureName( 0 )
//{
//	char * const	pDummyData	= new char[ tw*th*3 ];
//	if ( !pDummyData ) throw std::bad_alloc();
//
//	glGenTextures( 1, (GLuint *)&m_TextureName );
//	glBindTexture( GL_TEXTURE_2D, m_TextureName );
//
//	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, tw, th, 0, GL_RGB, GL_UNSIGNED_BYTE, pDummyData );
//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
//	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
//
//	delete[] pDummyData;
//}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

Mirror::~Mirror()
{
	delete m_pMaterial;
	delete m_pTexture;
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

/// @param	camera	The camera used in the scene
///
/// @note	The following states may be set by this function and are necessary for the reflection:
///				- glFrontFace( GL_CW )
///				- glMatrixMode( GL_MODELVIEW )
///				- glFrustum( ... )

bool Mirror::Begin( Glx::Camera const & camera )
{
	Vector3 const		mirrorNormal	= m_Frame.GetZAxis();
	Point const			mirrorPosition	= m_Frame.GetTranslation();
	Plane const			mirrorPlane( mirrorNormal, Dot( mirrorNormal, mirrorPosition ) );
	Point const			cameraPosition	= camera.GetPosition();
	float const			cameraDistance	= mirrorPlane.DirectedDistance( cameraPosition );

	// If the camera is in a position to see a reflection, then set up the reflection, otherwise do nothing

	if ( cameraDistance > 0.0f )
	{
		Quaternion const	mirrorRotation	= m_Frame.GetRotation();	// Orientation of the mirror

		// Save and set the viewport parameters

		glGetIntegerv( GL_VIEWPORT, (GLint *)&m_SavedViewportParameters );
		glViewport( 0, 0, m_pTexture->GetWidth(), m_pTexture->GetHeight() );

		// Save and set the projection matrix

		glMatrixMode( GL_PROJECTION );
		glPushMatrix();
		glLoadIdentity();

		// Offset in the plane's space from the projection of the camera origin to the mirror origin 
		Vector3 const	mirrorOffset	= ( mirrorPosition - mirrorPlane.Project( cameraPosition ) ).Rotate( -mirrorRotation );

		glFrustum( mirrorOffset.m_X - m_MirrorWidth * 0.5f,  mirrorOffset.m_X + m_MirrorWidth * 0.5f,
				   mirrorOffset.m_Y - m_MirrorHeight * 0.5f, mirrorOffset.m_Y + m_MirrorHeight * 0.5f,
				   std::max( cameraDistance, camera.GetNearDistance() ), camera.GetFarDistance() );

		// Save the modelview matrix

		glMatrixMode( GL_MODELVIEW );
		glPushMatrix();
		glLoadIdentity();

		glMultMatrixf( &Matrix44( (-mirrorRotation).GetRotationMatrix33() ).m_M[0][0] );
		glTranslatef( -cameraPosition.m_X, -cameraPosition.m_Y, -cameraPosition.m_Z );
//		glMultMatrixf( &Matrix44( mirrorRotation.GetRotationMatrix33() ).m_M[0][0] );
//		glTranslatef( 0.0f, 0.0f, -2.0*mirrorPlane.m_D );
//		glMultMatrixf( &Matrix44( mirrorRotation.GetRotationMatrix33() ).m_M[0][0] );
//		glScalef( 1.0f, 1.0f, -1.0f );
		glMultMatrixf( &Matrix44( mirrorPlane.GetReflectionMatrix() ).m_M[0][0] );

	//	Matrix44	modelView;
	//	glGetDoublev( GL_MODELVIEW_MATRIX, &modelView.m_M[0][0] );

		// Because of the reflection, the winding order of front faces are reversed

		glFrontFace( GL_CW );

		m_IsReflecting = true;
	}
	else
	{
		m_IsReflecting = false;
	}

	return m_IsReflecting;
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

/// @note	The following states may be set by this function:
///				- glDisable( GL_TEXTURE_2D )
///				- glDisable( GL_TEXTURE_1D )
///				- glDisable( GL_LIGHTING )
///				- glMatrixMode( GL_MODELVIEW )
///				- glFrontFace( GL_CCW )
///				- glBindTexture( GL_TEXTURE_2D, ... )
///				- glViewport( ... )

void Mirror::End()
{
	// If the mirror is reflecting, then copy the framebuffer to thereflection texture and restore the state.
	// Otherwise, there is nothing to do.

	if ( m_IsReflecting )
	{
		// Copy the image to the texture

		Glx::Disable( GL_TEXTURE_2D );
		Glx::Disable( GL_TEXTURE_1D );
		Glx::Disable( GL_LIGHTING );
		m_pTexture->Apply();
		glCopyTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_pTexture->GetWidth(), m_pTexture->GetHeight() );

		// Restore the viewport

		glViewport( m_SavedViewportParameters[ 0 ],
					m_SavedViewportParameters[ 1 ],
					m_SavedViewportParameters[ 2 ],
					m_SavedViewportParameters[ 3 ] );

		// Restore the projection

		glMatrixMode( GL_PROJECTION );
		glPopMatrix();

		// Restore the modelview

		glMatrixMode( GL_MODELVIEW );
		glPopMatrix();

		// Clear the depth buffer

		glDepthMask( GL_TRUE );
		glClear( GL_DEPTH_BUFFER_BIT );

		// Restore the winding order

		glFrontFace( GL_CCW );

		m_IsReflecting = false;
	}
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

/// @note	The following states are set by this function:
///				- glShadeModel( ... )
///				- glMaterialfv( m_Face, GL_AMBIENT_AND_DIFFUSE, ... )
///				- glMaterialfv( m_Face, GL_SPECULAR, ... )
///				- glMaterialf( m_Face, GL_SHININESS, ... )
///				- glMaterialfv( m_Face, GL_EMISSION, ... )
///				- glEnable( GL_TEXTURE_2D ), if textured
///				- glDisable( GL_TEXTURE_2D ), if not textured
///				- glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, ... )
///				- glBindTexture( GL_TEXTURE_2D, ... )

void Mirror::Apply() const
{
	m_pMaterial->Apply();

	glBegin( GL_QUADS );

	glNormal3f( 0.0f, 0.0f, 1.0f );

	glTexCoord2f( 0.0f, 0.0f );
	glVertex3f( -m_MirrorWidth * 0.5f, -m_MirrorHeight * 0.5f, 0.0f );

	glTexCoord2f( 1.0f, 0.0f );
	glVertex3f(  m_MirrorWidth * 0.5f, -m_MirrorHeight * 0.5f, 0.0f );

	glTexCoord2f( 1.0f, 1.0f );
	glVertex3f(  m_MirrorWidth * 0.5f,  m_MirrorHeight * 0.5f, 0.0f );

	glTexCoord2f( 0.0f, 1.0f );
	glVertex3f( -m_MirrorWidth * 0.5f,  m_MirrorHeight * 0.5f, 0.0f );

	glEnd();
}


} // namespace GlObjects
