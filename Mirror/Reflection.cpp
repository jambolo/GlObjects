/** @file *//********************************************************************************************************

                                                     Reflection.cpp

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/GlObjects/Mirror/Reflection.cpp#1 $

	$NoKeywords: $

 ********************************************************************************************************************/


#include "Reflection.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <gl/gl.h>

#include <new>

#include "Glx/Camera.h"
#include "Glx/Enable.h"
#include "Math/Plane.h"
#include "Math/Matrix44.h"
#include "Math/Point.h"


namespace GlObjects
{


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

/// @param	position	A point on the reflection plane
/// @param	normal		The normal to the reflection plane. Must be a unit vector.

Reflection::Reflection( Vector3 const & position, Vector3 const & normal )
	: m_Plane( normal, Dot( normal, position ) ),
	m_IsReflecting( false )
{
	assert( normal.IsNormalized() );
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

Reflection::~Reflection()
{
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

/// This function basically transforms the model-view matrix so that the locations of points in the world are
/// reflected by the reflection plane. It also sets up a clip plane so that everything behind the reflection plane
/// is clipped.
///
/// @param	camera	The camera used in the scene
///
/// @return			@c true if the camera can see the reflection and the reflection is active.
///
/// @note	The following states may be set by this function and are necessary for the reflection:
///				- glClipPlane( GL_CLIP_PLANE0, ... )
///				- glEnable( GL_CLIP_PLANE0 )
///				- glFrontFace( GL_CW )
///				- glMatrixMode( GL_MODELVIEW )

bool Reflection::Begin( Glx::Camera const & camera )
{
	// If the camera is in a position to see a reflection, then set up for the reflection, otherwise do nothing.

	if ( Distance( m_Plane, camera.GetPosition() ) > 0.0f )
	{
		// Save the modelview

		glMatrixMode( GL_MODELVIEW );
		glPushMatrix();

		// Reflect the world

		glMultMatrixf( &Matrix44( m_Plane.GetReflectionMatrix() ).m_M[0][0] );

		// Clip everything behind the reflection

		GLdouble	clip[ 4 ]	=	{ m_Plane.m_N.m_X, m_Plane.m_N.m_Y, m_Plane.m_N.m_Z, m_Plane.m_D };
		glClipPlane( GL_CLIP_PLANE0, clip );
		Glx::Enable( GL_CLIP_PLANE0 );

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

/// This function restores the renderer state to non-reflecting. If the reflection is not active (@c Begin was never
/// called, @c Begin returned @c false, or @c End has already been called), then this function does nothing.
///
/// @note	The following states may be set by this function:
///				- glDisable( GL_CLIP_PLANE0 );
///				- glFrontFace( GL_CCW )
///				- glDepthMask( GL_TRUE )
///				- glClear( GL_DEPTH_BUFFER_BIT );
///				- glMatrixMode( GL_MODELVIEW )

void Reflection::End()
{
	// If the reflection is active, then deactivate it. Otherwise, do nothing.

	if ( m_IsReflecting )
	{
		// Disable the clip plane

		Glx::Disable( GL_CLIP_PLANE0 );

		// Clear the depth buffer

		glDepthMask( GL_TRUE );
		glClear( GL_DEPTH_BUFFER_BIT );

		// Restore the modelview

		glMatrixMode( GL_MODELVIEW );
		glPopMatrix();

		// Restore the winding order

		glFrontFace( GL_CCW );

		// The reflection is no longer active.

		m_IsReflecting = false;
	}
}


} // namespace GlObjects
